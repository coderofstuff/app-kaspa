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
#include <stdbool.h>  // bool
#include "./types.h"
#include "./utils.h"
#include "../globals.h"
#include "../crypto.h"

bool tx_validate_parsed_transaction(transaction_t* tx) {
    // Invalid output length
    if (tx->tx_output_len > 2 || tx->tx_output_len < 1) {
        return false;
    }

    // There must be 1 or more inputs
    if (tx->tx_input_len < 1) {
        return false;
    }

    // Change address will always be output[1] if it exists
    if (tx->tx_output_len == 2) {
        transaction_output_t change_output = tx->tx_outputs[1];

        if (change_output.script_public_key[0] != 0x20) {
            // Change address can only be SCHNORR address and it's not
            return false;
        }

        uint8_t change_address_pubkey[32] = {0};

        // We can safely assume script_public_key has exactly 34 bytes. This
        // was validated when we deserialized the data.
        memmove(change_address_pubkey, change_output.script_public_key + 1, 32);

        // Forcing these values. path[3] and path[4]
        // would've been set by transaction_deserialize
        G_context.bip32_path[0] = 0x8000002C;
        G_context.bip32_path[1] = 0x8001b207;
        G_context.bip32_path[2] = 0x80000000;

        G_context.bip32_path_len = 5;

        if (!crypto_validate_public_key(G_context.bip32_path,
                                        G_context.bip32_path_len,
                                        change_address_pubkey)) {
            return false;
        }
    }

    // sum(input.values) >= sum(output.values)
    uint64_t input_total = 0;
    uint64_t output_total = 0;

    for (size_t i = 0; i < tx->tx_input_len; i++) {
        input_total += tx->tx_inputs[i].value;
    }

    for (size_t o = 0; o < tx->tx_output_len; o++) {
        output_total += tx->tx_outputs[o].value;
    }

    if (input_total < output_total) {
        return false;
    }

    return true;
}