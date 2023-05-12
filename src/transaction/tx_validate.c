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
        transaction_input_t first_utxo = tx->tx_inputs[0];

        if (change_output.script_public_key[0] != 0x20) {
            // Change address can only be SCHNORR address and it's not
            return false;
        }

        uint8_t change_address_pubkey[32] = {0};

        // We can safely assume script_public_key has exactly 34 bytes. This
        // was validated when we deserialized the data.
        memmove(change_address_pubkey, change_output.script_public_key + 1, 32);

        G_context.bip32_path[0] = 0x8000002C;
        G_context.bip32_path[1] = 0x8001b207;
        G_context.bip32_path[2] = 0x80000000;
        G_context.bip32_path[3] = (uint32_t)(first_utxo.address_type);
        G_context.bip32_path[4] = first_utxo.address_index;

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