#pragma once

#include <stdint.h>   // uint*_t
#include <stddef.h>   // size_t
#include <stdbool.h>  // bool

#include "types.h"

/**
 * Given a public key and the address type,
 * compress the public key into "out"
 * -> 32 bytes of X coord if type == SCHNORR
 * -> 1 byte of 0x02 if Y coord is odd, 0x03 if Y coord is even
 *    then 32 bytes of X coord if type == ECDSA
 * @return the size of the compressed key
 */
size_t compress_public_key(const uint8_t public_key[static 64], address_type_e address_type, uint8_t *out);

/**
 * Convert public key to address.
 *
 * address = Bech32(public_key)[0:31] (67 bytes) => "kaspa:" + 61 bytes
 *
 * @param[in]  public_key
 *   Pointer to byte buffer with public key.
 *   The public key is represented as 64 bytes with 32 bytes for
 *   each coordinate.
 * @param[in] address_type
 * @param[out] out
 *   Pointer to output byte buffer for address.
 * @param[in]  out_len
 *   Length of output byte buffer.
 *
 * @return true if success, false otherwise.
 *
 */
bool address_from_pubkey(const uint8_t public_key[static 64], address_type_e address_type, uint8_t *out, size_t out_len);
