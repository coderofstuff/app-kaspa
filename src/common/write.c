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

void write_u16_be(uint8_t *ptr, size_t offset, uint16_t value) {
    ptr[offset + 0] = (uint8_t)(value >> 8);
    ptr[offset + 1] = (uint8_t)(value >> 0);
}

void write_u32_be(uint8_t *ptr, size_t offset, uint32_t value) {
    ptr[offset + 0] = (uint8_t)(value >> 24);
    ptr[offset + 1] = (uint8_t)(value >> 16);
    ptr[offset + 2] = (uint8_t)(value >> 8);
    ptr[offset + 3] = (uint8_t)(value >> 0);
}

void write_u64_be(uint8_t *ptr, size_t offset, uint64_t value) {
    ptr[offset + 0] = (uint8_t)(value >> 56);
    ptr[offset + 1] = (uint8_t)(value >> 48);
    ptr[offset + 2] = (uint8_t)(value >> 40);
    ptr[offset + 3] = (uint8_t)(value >> 32);
    ptr[offset + 4] = (uint8_t)(value >> 24);
    ptr[offset + 5] = (uint8_t)(value >> 16);
    ptr[offset + 6] = (uint8_t)(value >> 8);
    ptr[offset + 7] = (uint8_t)(value >> 0);
}

void write_u16_le(uint8_t *ptr, size_t offset, uint16_t value) {
    ptr[offset + 0] = (uint8_t)(value >> 0);
    ptr[offset + 1] = (uint8_t)(value >> 8);
}

void write_u32_le(uint8_t *ptr, size_t offset, uint32_t value) {
    ptr[offset + 0] = (uint8_t)(value >> 0);
    ptr[offset + 1] = (uint8_t)(value >> 8);
    ptr[offset + 2] = (uint8_t)(value >> 16);
    ptr[offset + 3] = (uint8_t)(value >> 24);
}

void write_u64_le(uint8_t *ptr, size_t offset, uint64_t value) {
    ptr[offset + 0] = (uint8_t)(value >> 0);
    ptr[offset + 1] = (uint8_t)(value >> 8);
    ptr[offset + 2] = (uint8_t)(value >> 16);
    ptr[offset + 3] = (uint8_t)(value >> 24);
    ptr[offset + 4] = (uint8_t)(value >> 32);
    ptr[offset + 5] = (uint8_t)(value >> 40);
    ptr[offset + 6] = (uint8_t)(value >> 48);
    ptr[offset + 7] = (uint8_t)(value >> 56);
}
