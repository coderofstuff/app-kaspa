#include <string.h>
#include "deserialize.h"
#include "utils.h"
#include "types.h"
#include "../common/buffer.h"

parser_status_e transaction_output_deserialize(buffer_t *buf, transaction_output_t *txout) {
    // 8 bytes
    if (!buffer_read_u64(buf, &txout->value, BE)) {
        return OUTPUTS_PARSING_ERROR;
    }

    size_t script_len = (size_t) *(buf->ptr + buf->offset);
    // Can only be length 32 or 33. Fail it otherwise:
    if (script_len == 0x20 || script_len == 0x21) {
        if (!buffer_can_read(buf, script_len + 2)) {
            return OUTPUTS_PARSING_ERROR;
        }

        if (*(buf->ptr + buf->offset + script_len + 1) != 0xac) {
            return OUTPUTS_PARSING_ERROR;
        }

        memcpy(txout->script_public_key, buf->ptr + buf->offset, script_len + 2);
    } else {
        return OUTPUTS_PARSING_ERROR;
    }

    if (!buffer_seek_cur(buf, script_len + 2)) {
        return OUTPUTS_PARSING_ERROR;
    }

    // Total: 8 + 32|33 = 40|41 bytes
    return buf->size - buf->offset == 0 ? PARSING_OK : OUTPUTS_PARSING_ERROR;
}

parser_status_e transaction_input_deserialize(buffer_t *buf, transaction_input_t *txin) {
    // 8 bytes
    if (!buffer_read_u64(buf, &txin->value, BE)) {
        return INPUTS_PARSING_ERROR;
    }

    if (!buffer_can_read(buf, 32)) {
        // Not enough input
        return INPUTS_PARSING_ERROR;
    }

    // 32 bytes
    memcpy(txin->tx_id, buf->ptr, 32);

    if (!buffer_seek_cur(buf, 32)) {
        return INPUTS_PARSING_ERROR;
    }

    uint8_t address_type = -1;
    // 1 byte
    if (!buffer_read_u8(buf, &address_type)) {
        return INPUTS_PARSING_ERROR;
    }

    uint32_t address_index = -1;
    // 4 bytes
    if (!buffer_read_u32(buf, &address_index, BE)) {
        return INPUTS_PARSING_ERROR;
    }

    if (address_type < 0 || address_index < 0) {
        return INPUTS_PARSING_ERROR;
    }

    txin->derivation_path[0] = (uint32_t) address_type;
    txin->derivation_path[1] = address_index;

    // 1 byte
    if (!buffer_read_u8(buf, &txin->index)) {
        return INPUTS_PARSING_ERROR;
    }

    // Total: 46 bytes
    return buf->size - buf->offset == 0 ? PARSING_OK : INPUTS_PARSING_ERROR;
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

    // Must be 1 or 2 outputs, must match the number of outputs we parsed
    if (tx->tx_output_len < 1 || tx->tx_output_len > 2 || n_output != tx->tx_output_len) {
        return OUTPUTS_LENGTH_PARSING_ERROR;
    }

    uint8_t n_input = 0;
    if (!buffer_read_u8(buf, &n_input)) {
        return INPUTS_LENGTH_PARSING_ERROR;
    }

    // Must be at least 1, must match the number of inputs we parsed
    if (tx->tx_input_len < 1 || n_input != tx->tx_input_len) {
        return INPUTS_LENGTH_PARSING_ERROR;
    }

    uint32_t lastbits = 0;
    int diff = buf->size - buf->offset;
    if (buf->size - buf->offset == 4) {
        buffer_read_u32(buf, &lastbits, BE);
    } else {
        lastbits = buf->size - buf->offset;
    }

    return diff == 0 ? PARSING_OK : lastbits;
}
