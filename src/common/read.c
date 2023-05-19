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

#include <stdint.h>  // uint*_t
#include <stddef.h>  // size_t

uint16_t read_u16_be(const uint8_t *ptr, size_t offset) {
    return (uint16_t) ptr[offset + 0] << 8 |  //
           (uint16_t) ptr[offset + 1] << 0;
}

uint32_t read_u32_be(const uint8_t *ptr, size_t offset) {
    return (uint32_t) ptr[offset + 0] << 24 |  //
           (uint32_t) ptr[offset + 1] << 16 |  //
           (uint32_t) ptr[offset + 2] << 8 |   //
           (uint32_t) ptr[offset + 3] << 0;
}

uint64_t read_u64_be(const uint8_t *ptr, size_t offset) {
    return (uint64_t) ptr[offset + 0] << 56 |  //
           (uint64_t) ptr[offset + 1] << 48 |  //
           (uint64_t) ptr[offset + 2] << 40 |  //
           (uint64_t) ptr[offset + 3] << 32 |  //
           (uint64_t) ptr[offset + 4] << 24 |  //
           (uint64_t) ptr[offset + 5] << 16 |  //
           (uint64_t) ptr[offset + 6] << 8 |   //
           (uint64_t) ptr[offset + 7] << 0;
}

uint16_t read_u16_le(const uint8_t *ptr, size_t offset) {
    return (uint16_t) ptr[offset + 0] << 0 |  //
           (uint16_t) ptr[offset + 1] << 8;
}

uint32_t read_u32_le(const uint8_t *ptr, size_t offset) {
    return (uint32_t) ptr[offset + 0] << 0 |   //
           (uint32_t) ptr[offset + 1] << 8 |   //
           (uint32_t) ptr[offset + 2] << 16 |  //
           (uint32_t) ptr[offset + 3] << 24;
}

uint64_t read_u64_le(const uint8_t *ptr, size_t offset) {
    return (uint64_t) ptr[offset + 0] << 0 |   //
           (uint64_t) ptr[offset + 1] << 8 |   //
           (uint64_t) ptr[offset + 2] << 16 |  //
           (uint64_t) ptr[offset + 3] << 24 |  //
           (uint64_t) ptr[offset + 4] << 32 |  //
           (uint64_t) ptr[offset + 5] << 40 |  //
           (uint64_t) ptr[offset + 6] << 48 |  //
           (uint64_t) ptr[offset + 7] << 56;
}
