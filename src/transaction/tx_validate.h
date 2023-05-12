#pragma once

#include <stdbool.h>  // bool
#include "./types.h"

/**
 * Check if the transaction as parsed follows the conventions set for it
 * - If a change output exists (output idx 1), the address MUST match the address of the first UTXO
 * - Sum of input values must be >= sum of output values
 * @param[in]  tx
 *   The transaction that received the parsed data to validate
 * @return true if the transaction follows conventions, false otherwise.
 */
bool tx_validate_parsed_transaction(transaction_t* tx);