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
#pragma once

#include <stdint.h>   // uint*_t
#include <stddef.h>   // size_t
#include <stdbool.h>  // bool

/**
 * Size of value represented as Bitcoin-like varint.
 *
 * @see https://en.bitcoin.it/wiki/Protocol_documentation#Variable_length_integer
 *
 * @param[in] value
 *   64-bit unsigned integer to compute varint size.
 *
 * @return number of bytes to write value as varint (1, 3, 5 or 9 bytes).
 *
 */
uint8_t varint_size(uint64_t value);

/**
 * Read Bitcoin-like varint from byte buffer.
 *
 * @see https://en.bitcoin.it/wiki/Protocol_documentation#Variable_length_integer
 *
 * @param[in]  in
 *   Pointer to input byte buffer.
 * @param[in]  in_len
 *   Length of the input byte buffer.
 * @param[out] value
 *   Pointer to 64-bit unsigned integer to output varint.
 *
 * @return number of bytes read (1, 3, 5 or 9 bytes), -1 otherwise.
 *
 */
int varint_read(const uint8_t *in, size_t in_len, uint64_t *value);

/**
 * Write Bitcoin-like varint to byte buffer.
 *
 * @see https://en.bitcoin.it/wiki/Protocol_documentation#Variable_length_integer
 *
 * @param[out] out
 *   Pointer to output byte buffer.
 * @param[in]  offset
 *   Offset in the output byte buffer.
 * @param[in]  value
 *   64-bit unsigned integer to write as varint.
 *
 * @return number of bytes written (1, 3, 5 or 9 bytes), -1 otherwise.
 *
 */
int varint_write(uint8_t *out, size_t offset, uint64_t value);
