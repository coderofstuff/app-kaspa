#include <stdint.h>
#include <stdbool.h>  // bool
#include "./types.h"
#include "./utils.h"
#include "../globals.h"
#include "../crypto.h"

static bool tx_validate_input(transaction_input_t* txin) {
    if (txin->address_type != RECEIVE && txin->address_type != CHANGE) {
        return false;
    }

    return true;
}

static bool tx_validate_output(transaction_output_t* txout) {
    if (txout->has_path) {
        if (txout->address_type != RECEIVE && txout->address_type != CHANGE) {
            return false;
        }
    } else {
        uint8_t script_len = txout->script_public_key[0];

        if (script_len != 0x20 && script_len != 0x21) {
            return false;
        }
    }

    return true;
}

static bool tx_validate_fees(transaction_t* tx) {
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

static bool tx_validate_change_address(transaction_t* tx) {
    // Change address will always be output[1] if it exists
    return tx->tx_output_len == 1 || (tx->tx_output_len == 2 && tx->tx_outputs[1].has_path);
}

bool tx_validate_parsed_transaction(transaction_t* tx) {
    // Invalid output length
    if (tx->tx_output_len > 2 || tx->tx_output_len < 1) {
        return false;
    }

    // There must be 1 or more inputs
    if (tx->tx_input_len < 1) {
        return false;
    }

    for (size_t i = 0; i < tx->tx_input_len; i++) {
        if (!tx_validate_input(&tx->tx_inputs[i])) {
            return false;
        }
    }

    for (size_t o = 0; o < tx->tx_output_len; o++) {
        if (!tx_validate_output(&tx->tx_outputs[o])) {
            return false;
        }
    }

    if (!tx_validate_change_address(tx)) {
        return false;
    }

    if (!tx_validate_fees(tx)) {
        return false;
    }

    return true;
}