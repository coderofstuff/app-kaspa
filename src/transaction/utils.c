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
    memmove(public_key, in_script_public_key + 1, data_len);

    address_type_e type = data_len == 0x20 ? SCHNORR : ECDSA;
    size_t address_len = type == SCHNORR ? SCHNORR_ADDRESS_LEN : ECDSA_ADDRESS_LEN;
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