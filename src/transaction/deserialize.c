#include <string.h>
#include "deserialize.h"
#include "utils.h"
#include "types.h"
#include "../common/buffer.h"

parser_status_e transaction_output_deserialize(buffer_t *buf, transaction_output_t *txout) {
    // 8 bytes
    if (!buffer_read_u64(buf, &txout->value, BE)) {
        return OUTPUT_VALUE_PARSING_ERROR;
    }

    size_t script_len = (size_t) * (buf->ptr + buf->offset);
    // Can only be length 32 or 33. Fail it otherwise:
    if (script_len == 0x20 || script_len == 0x21) {
        if (!buffer_can_read(buf, script_len + 2)) {
            return OUTPUT_SCRIPT_PUBKEY_PARSING_ERROR;
        }

        uint8_t sig_op_code = *(buf->ptr + buf->offset + script_len + 1);

        if ((script_len == 0x20 && sig_op_code != OP_CHECKSIG) ||
            (script_len == 0x21 && sig_op_code != OP_CHECKSIGECDSA)) {
            return OUTPUT_SCRIPT_PUBKEY_PARSING_ERROR;
        }

        memcpy(txout->script_public_key, buf->ptr + buf->offset, script_len + 2);
    } else {
        return OUTPUT_SCRIPT_PUBKEY_PARSING_ERROR;
    }

    if (!buffer_seek_cur(buf, script_len + 2)) {
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

parser_status_e transaction_deserialize(buffer_t *buf, transaction_t *tx) {
    // Version, 2 bytes
    if (!buffer_read_u16(buf, &tx->version, BE)) {
        return VERSION_PARSING_ERROR;
    }

    uint8_t n_output = 0;
    if (!buffer_read_u8(buf, &n_output)) {
        return OUTPUTS_LENGTH_PARSING_ERROR;
    }

    tx->tx_output_len = n_output;

    // Must be 1 or 2 outputs, must match the number of outputs we parsed
    if (tx->tx_output_len < 1 || tx->tx_output_len > 2) {
        return OUTPUTS_LENGTH_PARSING_ERROR;
    }

    uint8_t n_input = 0;
    if (!buffer_read_u8(buf, &n_input)) {
        return INPUTS_LENGTH_PARSING_ERROR;
    }

    tx->tx_input_len = n_input;

    // Must be at least 1, must match the number of inputs we parsed
    if (tx->tx_input_len < 1 || tx->tx_input_len > MAX_INPUT_COUNT) {
        return INPUTS_LENGTH_PARSING_ERROR;
    }

    return buf->size - buf->offset == 0 ? PARSING_OK : HEADER_PARSING_ERROR;
}
