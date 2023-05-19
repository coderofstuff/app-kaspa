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
 * Read 2 bytes as Big Endian from byte buffer.
 *
 * @param[in] ptr
 *   Pointer to byte buffer.
 * @param[in] offset
 *   Offset in the byte buffer.
 *
 * @return 2 bytes value read from buffer.
 *
 */
uint16_t read_u16_be(const uint8_t *ptr, size_t offset);

/**
 * Read 4 bytes as Big Endian from byte buffer.
 *
 * @param[in] ptr
 *   Pointer to byte buffer.
 * @param[in] offset
 *   Offset in the byte buffer.
 *
 * @return 4 bytes value read from buffer.
 *
 */
uint32_t read_u32_be(const uint8_t *ptr, size_t offset);

/**
 * Read 8 bytes as Big Endian from byte buffer.
 *
 * @param[in] ptr
 *   Pointer to byte buffer.
 * @param[in] offset
 *   Offset in the byte buffer.
 *
 * @return 8 bytes value read from buffer.
 *
 */
uint64_t read_u64_be(const uint8_t *ptr, size_t offset);

/**
 * Read 2 bytes as Little Endian from byte buffer.
 *
 * @param[in] ptr
 *   Pointer to byte buffer.
 * @param[in] offset
 *   Offset in the byte buffer.
 *
 * @return 2 bytes value read from buffer.
 *
 */
uint16_t read_u16_le(const uint8_t *ptr, size_t offset);

/**
 * Read 4 bytes as Little Endian from byte buffer.
 *
 * @param[in] ptr
 *   Pointer to byte buffer.
 * @param[in] offset
 *   Offset in the byte buffer.
 *
 * @return 4 bytes value read from buffer.
 *
 */
uint32_t read_u32_le(const uint8_t *ptr, size_t offset);

/**
 * Read 8 bytes as Little Endian from byte buffer.
 *
 * @param[in] ptr
 *   Pointer to byte buffer.
 * @param[in] offset
 *   Offset in the byte buffer.
 *
 * @return 8 bytes value read from buffer.
 *
 */
uint64_t read_u64_le(const uint8_t *ptr, size_t offset);
