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

#include <stdint.h>  // uint*_t
#include <stddef.h>  // size_t

/**
 * Write 16-bit unsigned integer value as Big Endian.
 *
 * @param[out] ptr
 *   Pointer to output byte buffer.
 * @param[in]  offset
 *   Offset in the output byte buffer.
 * @param[in]  value
 *   16-bit unsigned integer to write in output byte buffer as Big Endian.
 *
 */
void write_u16_be(const uint8_t *ptr, size_t offset, uint16_t value);

/**
 * Write 32-bit unsigned integer value as Big Endian.
 *
 * @param[out] ptr
 *   Pointer to output byte buffer.
 * @param[in]  offset
 *   Offset in the output byte buffer.
 * @param[in]  value
 *   32-bit unsigned integer to write in output byte buffer as Big Endian.
 *
 */
void write_u32_be(uint8_t *ptr, size_t offset, uint32_t value);

/**
 * Write 64-bit unsigned integer value as Big Endian.
 *
 * @param[out] ptr
 *   Pointer to output byte buffer.
 * @param[in]  offset
 *   Offset in the output byte buffer.
 * @param[in]  value
 *   64-bit unsigned integer to write in output byte buffer as Big Endian.
 *
 */
void write_u64_be(uint8_t *ptr, size_t offset, uint64_t value);

/**
 * Write 16-bit unsigned integer value as Little Endian.
 *
 * @param[out] ptr
 *   Pointer to output byte buffer.
 * @param[in]  offset
 *   Offset in the output byte buffer.
 * @param[in]  value
 *   16-bit unsigned integer to write in output byte buffer as Little Endian.
 *
 */
void write_u16_le(uint8_t *ptr, size_t offset, uint16_t value);

/**
 * Write 32-bit unsigned integer value as Little Endian.
 *
 * @param[out] ptr
 *   Pointer to output byte buffer.
 * @param[in]  offset
 *   Offset in the output byte buffer.
 * @param[in]  value
 *   32-bit unsigned integer to write in output byte buffer as Little Endian.
 *
 */
void write_u32_le(uint8_t *ptr, size_t offset, uint32_t value);

/**
 * Write 64-bit unsigned integer value as Little Endian.
 *
 * @param[out] ptr
 *   Pointer to output byte buffer.
 * @param[in]  offset
 *   Offset in the output byte buffer.
 * @param[in]  value
 *   64-bit unsigned integer to write in output byte buffer as Little Endian.
 *
 */
void write_u64_le(uint8_t *ptr, size_t offset, uint64_t value);
