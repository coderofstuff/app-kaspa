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
#include <stdbool.h>  // bool

#include "types.h"

/**
 * Check if memo is encoded using ASCII characters.
 *
 * @param[in] memo
 *   Pointer to input byte buffer.
 * @param[in] memo_len
 *   Length of input byte buffer.
 *
 * @return true if success, false otherwise.
 *
 */
bool transaction_utils_check_encoding(const uint8_t* memo, uint64_t memo_len);

/**
 * Translates a given output Script Public Key to it's Kaspa Address.
 *
 * @param[out] out_address
 *   Pointer to the buffer to write the address to
 * @param[in] out_len length of output buffer
 * @param[in] in_script_public_key
 *   Pointer to the buffer to read script_public_key from
 * @param[in] script_len length of script output buffer
 */
bool script_public_key_to_address(uint8_t* out_address,
                                  size_t out_len,
                                  uint8_t* in_script_public_key,
                                  size_t script_len);

/**
 * Calculate the fees by checking the difference between inputs and outputs
 * @param[in] inputs
 *   Pointer to tx input array.
 * @param[in] input_len
 *   Length of tx input byte buffer.
 * @param[in] outputs
 *   Pointer to tx output array.
 * @param[in] output_len
 *   Length of tx output byte buffer.
 */
uint64_t calc_fees(transaction_input_t* inputs,
                   size_t input_len,
                   transaction_output_t* outputs,
                   size_t output_len);
