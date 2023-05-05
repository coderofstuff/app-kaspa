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

int calc_fees(transaction_input_t* inputs,
              size_t input_len,
              transaction_output_t* outputs,
              size_t output_len) {
    int fees = 0;

    for (size_t i = 0; i < input_len; i++) {
        fees += inputs[i].value;
    }

    for (size_t i = 0; i < output_len; i++) {
        fees -= outputs[i].value;
    }

    return fees;
}
