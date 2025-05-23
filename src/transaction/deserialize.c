/*****************************************************************************
 * MIT License
 *
 * Copyright (c) 2023 coderofstuff
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/
#include <string.h>
#include "deserialize.h"
#include "utils.h"
#include "types.h"
#include "buffer.h"

parser_status_e transaction_output_deserialize(buffer_t *buf, transaction_output_t *txout) {
    // 8 bytes
    if (!buffer_read_u64(buf, &txout->value, BE)) {
        return OUTPUT_VALUE_PARSING_ERROR;
    }

    if (!buffer_can_read(buf, 1)) {
        return OUTPUT_SCRIPT_PUBKEY_PARSING_ERROR;
    }

    uint8_t script_len = (uint8_t) * (buf->ptr + buf->offset);

    if (script_len == OP_BLAKE2B) {
        // P2SH = 0xaa + 0x20 + (script hash) + 0x87
        // Total length = 35
        // script len is actually the second byte if the first one is 0xaa
        if (!buffer_can_read(buf, 2)) {
            return OUTPUT_SCRIPT_PUBKEY_PARSING_ERROR;
        }

        script_len = (uint8_t) * (buf->ptr + buf->offset + 1);

        // For P2SH, we expect len to always be 0x20
        if (script_len != 0x20) {
            return OUTPUT_SCRIPT_PUBKEY_PARSING_ERROR;
        }

        if (!buffer_can_read(buf, script_len + 3)) {
            return OUTPUT_SCRIPT_PUBKEY_PARSING_ERROR;
        }

        uint8_t sig_op_code = *(buf->ptr + buf->offset + script_len + 2);

        // We expect the end to always be 0x87 for P2SH
        if (sig_op_code != OP_EQUAL) {
            return OUTPUT_SCRIPT_PUBKEY_PARSING_ERROR;
        }

        memcpy(txout->script_public_key, buf->ptr + buf->offset, script_len + 3);

        if (!buffer_seek_cur(buf, script_len + 3)) {
            return OUTPUT_SCRIPT_PUBKEY_PARSING_ERROR;
        }
    } else if (script_len == 0x20 || script_len == 0x21) {
        // P2PK
        // Can only be length 32 or 33. Fail it otherwise:
        if (!buffer_can_read(buf, script_len + 2)) {
            return OUTPUT_SCRIPT_PUBKEY_PARSING_ERROR;
        }

        uint8_t sig_op_code = *(buf->ptr + buf->offset + script_len + 1);

        if ((script_len == 0x20 && sig_op_code != OP_CHECKSIG) ||
            (script_len == 0x21 && sig_op_code != OP_CHECKSIGECDSA)) {
            return OUTPUT_SCRIPT_PUBKEY_PARSING_ERROR;
        }

        memcpy(txout->script_public_key, buf->ptr + buf->offset, script_len + 2);

        if (!buffer_seek_cur(buf, script_len + 2)) {
            return OUTPUT_SCRIPT_PUBKEY_PARSING_ERROR;
        }
    } else {
        return OUTPUT_SCRIPT_PUBKEY_PARSING_ERROR;
    }

    // Total: 8 + 32|33 = 40|41 bytes
    return buf->size - buf->offset == 0 ? PARSING_OK : OUTPUT_PARSING_ERROR;
}

parser_status_e transaction_input_deserialize(buffer_t *buf, transaction_input_t *txin) {
    // 8 bytes
    if (!buffer_read_u64(buf, &txin->value, BE)) {
        return INPUT_VALUE_PARSING_ERROR;
    }

    if (!buffer_can_read(buf, 32)) {
        // Not enough input
        return INPUT_TX_ID_PARSING_ERROR;
    }

    // 32 bytes
    memcpy(txin->tx_id, buf->ptr + buf->offset, 32);

    if (!buffer_seek_cur(buf, 32)) {
        return INPUT_TX_ID_PARSING_ERROR;
    }

    // 1 byte
    if (!buffer_read_u8(buf, &txin->address_type)) {
        return INPUT_ADDRESS_TYPE_PARSING_ERROR;
    }

    // 4 bytes
    if (!buffer_read_u32(buf, &txin->address_index, BE)) {
        return INPUT_ADDRESS_INDEX_PARSING_ERROR;
    }

    // 1 byte
    if (!buffer_read_u8(buf, &txin->index)) {
        return INPUT_INDEX_PARSING_ERROR;
    }

    // Total: 46 bytes
    return buf->size - buf->offset == 0 ? PARSING_OK : INPUT_PARSING_ERROR;
}

parser_status_e transaction_deserialize(buffer_t *buf, transaction_t *tx, uint32_t *bip32_path) {
    if (KASPA_MAX_BIP32_PATH_LEN < 5) {
        return HEADER_PARSING_ERROR;
    }

    uint8_t n_output = 0;
    uint8_t n_input = 0;
    uint8_t change_address_type = 0;
    uint32_t change_address_index = 0;

    // Version, 2 bytes
    if (!buffer_read_u16(buf, &tx->version, BE)) {
        return VERSION_PARSING_ERROR;
    }

    if (!buffer_read_u8(buf, &n_output)) {
        return OUTPUTS_LENGTH_PARSING_ERROR;
    }

    tx->tx_output_len = n_output;

    // Must be 1 or 2 outputs, must match the number of outputs we parsed
    if (tx->tx_output_len < 1 || tx->tx_output_len > 2) {
        return OUTPUTS_LENGTH_PARSING_ERROR;
    }

    if (!buffer_read_u8(buf, &n_input)) {
        return INPUTS_LENGTH_PARSING_ERROR;
    }

    tx->tx_input_len = n_input;

    // Must be at least 1, must match the number of inputs we parsed
    if (tx->tx_input_len < 1 || tx->tx_input_len > MAX_INPUT_COUNT) {
        return INPUTS_LENGTH_PARSING_ERROR;
    }

    if (!buffer_read_u8(buf, &change_address_type)) {
        return HEADER_PARSING_ERROR;
    }

    if (change_address_type != 0 && change_address_type != 1) {
        return HEADER_PARSING_ERROR;
    }

    if (!buffer_read_u32(buf, &change_address_index, BE)) {
        return HEADER_PARSING_ERROR;
    }

    if (!buffer_read_u32(buf, &tx->account, BE)) {
        return HEADER_PARSING_ERROR;
    }

    // Account must be hardened
    if (tx->account < 0x80000000) {
        return HEADER_PARSING_ERROR;
    }

    bip32_path[0] = 0x8000002C;
    bip32_path[1] = 0x8001b207;
    bip32_path[2] = tx->account;
    bip32_path[3] = (uint32_t) (change_address_type);
    bip32_path[4] = change_address_index;

    return buf->size - buf->offset == 0 ? PARSING_OK : HEADER_PARSING_ERROR;
}
