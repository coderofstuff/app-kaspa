#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool
#include <stddef.h>   // size_t
#include <string.h>   // memmove

#include "serialize.h"
#include "../common/write.h"
#include "../common/varint.h"

int transaction_serialize(const transaction_t *tx, uint8_t *out, size_t out_len) {
    size_t offset = 0;

    if (out_len < 4) {
        return -1;
    }

    write_u16_be(out, offset, tx->version);
    offset += 2;
    out[offset++] = tx->tx_output_len;
    out[offset++] = tx->tx_input_len;

    return (int) offset;
}

int transaction_input_serialize(const transaction_input_t *txin, uint8_t *out, size_t out_len) {
    size_t offset = 0;

    if (out_len < 46) {
        return -1;
    }

    write_u64_be(out, offset, txin->value);
    offset += 8;

    memcpy(out + offset, txin->tx_id, 32);
    offset += 32;

    out[offset++] = txin->address_type;

    write_u32_be(out, offset, txin->address_index);
    offset += 4;

    out[offset++] = txin->index;

    return (int) offset;
}

int transaction_output_serialize(const transaction_output_t *txout, uint8_t *out, size_t out_len) {
    size_t offset = 0;

    if (out_len < 41) {
        return -1;
    }

    write_u64_be(out, offset, txout->value);
    offset += 8;

    size_t script_len = txout->script_public_key[0];

    memcpy(out + offset, txout->script_public_key, script_len + 2);

    offset += script_len + 2;

    return (int) offset;
}
