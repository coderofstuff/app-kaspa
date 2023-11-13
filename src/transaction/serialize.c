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
#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool
#include <stddef.h>   // size_t
#include <string.h>   // memmove

#include "serialize.h"
#include "write.h"

int transaction_serialize(const transaction_t *tx, uint32_t *path, uint8_t *out, size_t out_len) {
    size_t offset = 0;

    if (out_len < 9) {
        return -1;
    }

    write_u16_be(out, offset, tx->version);
    offset += 2;
    out[offset++] = tx->tx_output_len;
    out[offset++] = tx->tx_input_len;

    out[offset++] = (uint8_t) path[3];

    write_u32_be(out, offset, path[4]);
    offset += 4;

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

    if (txout->script_public_key[0] == OP_BLAKE2B) {
        size_t script_len = txout->script_public_key[1];

        memcpy(out + offset, txout->script_public_key, script_len + 3);

        offset += script_len + 3;
    } else {
        size_t script_len = txout->script_public_key[0];

        memcpy(out + offset, txout->script_public_key, script_len + 2);

        offset += script_len + 2;
    }

    return (int) offset;
}
