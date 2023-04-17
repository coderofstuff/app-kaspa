#include "deserialize.h"
#include "utils.h"
#include "types.h"
#include "../common/buffer.h"

int deserialize_output(buffer_t *buf, transaction_output_t *txout) {
    if (!buffer_read_u64(buf, &txout->value, BE)) {
        return -70;
    }

    txout->script_public_key = (uint8_t *) (buf->ptr + buf->offset);

    if (!buffer_seek_cur(buf, 34)) {
        return -80;
    }

    return 1;
}

int deserialize_input(buffer_t *buf, transaction_input_t *txin) {
    if (!buffer_read_u64(buf, &txin->value, BE)) {
        return 0;
    }

    txin->tx_id = (uint8_t *) (buf->ptr + buf->offset);

    if (!buffer_seek_cur(buf, 32)) {
        return 0;
    }

    uint8_t address_type = -1;
    buffer_read_u8(buf, &address_type);

    uint32_t address_index = -1;
    buffer_read_u32(buf, &address_index, BE);

    if (address_type < 0 || address_index < 0) {
        return 0;
    }

    return 1;
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

    // Must be 1 or 2 outputs
    if (tx->tx_output_len < 1 || tx->tx_output_len > 2) {
        return OUTPUTS_LENGTH_PARSING_ERROR;
    }

    uint8_t n_input = 0;
    if (!buffer_read_u8(buf, &n_input)) {
        return INPUTS_LENGTH_PARSING_ERROR;
    }

    tx->tx_input_len = n_input;

    if (tx->tx_input_len < 1 || tx->tx_input_len > 2) {
        return INPUTS_LENGTH_PARSING_ERROR;
    }

    for (int i = 0; i < (int) tx->tx_output_len && i < 2; i++) {
        int res = deserialize_output(buf, &tx->tx_outputs[i]);
        if (res < 0) {
            return res;
        }
    }

    for (int i = 0; i < (int) tx->tx_input_len; i++) {
        if (!deserialize_input(buf, &tx->tx_inputs[i])) {
            return INPUTS_PARSING_ERROR;
        }
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
