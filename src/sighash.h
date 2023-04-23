#include <stdint.h>
#include "./transaction/types.h"

/**
 * Calculate the signature hash for the given transaction and input
 */
void calc_sighash(transaction_t* tx, transaction_input_t* txin, uint8_t* public_key, uint8_t* out_hash);
