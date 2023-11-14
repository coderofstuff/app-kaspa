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
#include <stdint.h>
#include <stdbool.h>

#include "constants.h"
#include "./import/blake2-impl.h"
#include "./import/blake2b.h"
#include "./personal_message.h"

static bool hash_init(blake2b_state* hash, size_t size, uint8_t* key, size_t key_len) {
    if (key == NULL && key_len != 0) {
        goto err;
    }

    if (size % 8 != 0 || size < 8 || size > 512) {
        goto err;
    }
    memset(hash, 0, sizeof(blake2b_state));

    size = size / 8;

    if (blake2b_init_key(hash, size, key, key_len) < 0) {
        goto err;
    }
    return true;

err:
    return false;
}

static bool hash_update(blake2b_state* hash, uint8_t* data, size_t len) {
    return blake2b_update(hash, data, len) == 0;
}

static bool hash_finalize(blake2b_state* hash, uint8_t* out, size_t out_len) {
    if (out_len < 32) {
        return false;
    }
    return blake2b_final(hash, out, 32) == 0;
}

bool hash_personal_message(uint8_t* message_bytes,
                           size_t message_byte_len,
                           uint8_t* out_hash,
                           size_t out_len) {
    if (out_len < 32) {
        return false;
    }

    blake2b_state inner_hash_writer;
    if (!hash_init(&inner_hash_writer, 256, (uint8_t*) MESSAGE_SIGNING_KEY, 26)) {
        return false;
    }

    if (!hash_update(&inner_hash_writer, message_bytes, message_byte_len)) {
        return false;
    }

    return hash_finalize(&inner_hash_writer, out_hash, out_len);
}
