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
 * @param[in] in_script_public_key
 *   Pointer to the buffer to read script_public_key from
 */
void script_public_key_to_address(uint8_t* out_address, uint8_t* in_script_public_key);

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
