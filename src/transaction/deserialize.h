#pragma once

#include "types.h"
#include "../common/buffer.h"

/**
 * Deserialize raw transaction in structure.
 *
 * @param[in, out] buf
 *   Pointer to buffer with serialized transaction.
 * @param[out]     tx
 *   Pointer to transaction structure.
 * @param[out]     bip32_path
 *   Pointer to bip32_path array where change address info will be set
 *
 * @return PARSING_OK if success, error status otherwise.
 *
 */
parser_status_e transaction_deserialize(buffer_t *buf, transaction_t *tx, uint32_t *bip32_path);

parser_status_e transaction_output_deserialize(buffer_t *buf, transaction_output_t *txout);

parser_status_e transaction_input_deserialize(buffer_t *buf, transaction_input_t *txin);
