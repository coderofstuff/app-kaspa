#include <string.h>
#include "deserialize.h"
#include "utils.h"
#include "types.h"
#include "../common/buffer.h"

parser_status_e transaction_output_deserialize(buffer_t *buf, transaction_output_t *txout) {
    if (!buffer_read_u64(buf, &txout->value, BE)) {
        return -70;
    }

    size_t len = (size_t) *(buf->ptr + buf->offset);
    // Can only be length 32 or 33. Fail it otherwise:
    if (len == 0x20 || len == 0x21) {
        memcpy(txout->script_public_key, buf->ptr + buf->offset, len + 2);
    } else {
        return -90;
    }

    if (!buffer_seek_cur(buf, len + 2)) {
        return -80;
    }

    return 1;
}

parser_status_e transaction_input_deserialize(buffer_t *buf, transaction_input_t *txin) {
    if (!buffer_read_u64(buf, &txin->value, BE)) {
        return INPUTS_PARSING_ERROR;
    }

    if (buf->offset + 32 > buf->size) {
        // Not enough input
        return INPUTS_PARSING_ERROR;
    }

    memcpy(txin->tx_id, buf->ptr, 32);

    if (!buffer_seek_cur(buf, 32)) {
        return INPUTS_PARSING_ERROR;
    }

    uint8_t address_type = -1;
    buffer_read_u8(buf, &address_type);

    uint32_t address_index = -1;
    buffer_read_u32(buf, &address_index, BE);

    if (address_type < 0 || address_index < 0) {
        return INPUTS_PARSING_ERROR;
    }

    txin->derivation_path[0] = (uint32_t) address_type;
    txin->derivation_path[1] = address_index;

    buffer_read_u8(buf, &txin->index);

    return PARSING_OK;
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
