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
#include <string.h>   // memmove

#include "address.h"
#include "types.h"

bool transaction_utils_check_encoding(const uint8_t* memo, uint64_t memo_len) {
    for (uint64_t i = 0; i < memo_len; i++) {
        if (memo[i] > 0x7F) {
            return false;
        }
    }

    return true;
}

void script_public_key_to_address(uint8_t* out_address, uint8_t* in_script_public_key) {
    uint8_t public_key[64] = {0};
    // public script keys begin with the length, followed by the amount of data
    size_t data_len = (size_t) in_script_public_key[0];
    address_type_e type = SCHNORR;
    size_t address_len = SCHNORR_ADDRESS_LEN;

    if (data_len == 0x21) {
        // We're dealing with ECDSA instead:
        type = ECDSA;
        address_len = ECDSA_ADDRESS_LEN;

        // This is either one of 0x02 or 0x03
        uint8_t parity = in_script_public_key[1];

        memmove(public_key, in_script_public_key + 2, 32);

        // Set the correct parity for the y-coord
        memset(public_key + 32, parity, 32);
    } else {
        memmove(public_key, in_script_public_key + 1, 32);
    }

    address_from_pubkey(public_key, type, out_address, address_len);
}

uint64_t calc_fees(transaction_input_t* inputs,
                   size_t input_len,
                   transaction_output_t* outputs,
                   size_t output_len) {
    uint64_t fees = 0;

    for (size_t i = 0; i < input_len; i++) {
        fees += inputs[i].value;
    }

    for (size_t i = 0; i < output_len; i++) {
        fees -= outputs[i].value;
    }

    return fees;
}
