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
#include <stddef.h>   // size_t
#include <stdbool.h>  // bool

#include "varint.h"
#include "write.h"
#include "read.h"

uint8_t varint_size(uint64_t value) {
    if (value <= 0xFC) {
        return 1;
    }

    if (value <= UINT16_MAX) {
        return 3;
    }

    if (value <= UINT32_MAX) {
        return 5;
    }

    return 9;  // <= UINT64_MAX
}

int varint_read(const uint8_t *in, size_t in_len, uint64_t *value) {
    if (in_len < 1) {
        return -1;
    }

    uint8_t prefix = in[0];

    if (prefix == 0xFD) {
        if (in_len < 3) {
            return -1;
        }
        *value = (uint64_t) read_u16_le(in, 1);
        return 3;
    }

    if (prefix == 0xFE) {
        if (in_len < 5) {
            return -1;
        }
        *value = (uint64_t) read_u32_le(in, 1);
        return 5;
    }

    if (prefix == 0xFF) {
        if (in_len < 9) {
            return -1;
        }
        *value = (uint64_t) read_u64_le(in, 1);
        return 9;
    }

    *value = (uint64_t) prefix;  // prefix <= 0xFC

    return 1;
}

int varint_write(uint8_t *out, size_t offset, uint64_t value) {
    uint8_t varint_len = varint_size(value);

    switch (varint_len) {
        case 1:
            out[offset] = (uint8_t) value;
            break;
        case 3:
            out[offset++] = 0xFD;
            write_u16_le(out, offset, (uint16_t) value);
            break;
        case 5:
            out[offset++] = 0xFE;
            write_u32_le(out, offset, (uint32_t) value);
            break;
        case 9:
            out[offset++] = 0xFF;
            write_u64_le(out, offset, (uint64_t) value);
            break;
        default:
            return -1;
    }

    return varint_len;
}
