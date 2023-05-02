#include <stdint.h>   // uint*_t
#include <stddef.h>   // size_t
#include <stdbool.h>  // bool
#include <string.h>   // memmove

#include "address.h"
#include "./import/cashaddr.h"

#include "transaction/types.h"

bool address_from_pubkey(const uint8_t public_key[static 64], uint8_t *out, size_t out_len) {
    uint8_t address[80] = {0};

    if (out_len < ADDRESS_LEN) {
        return false;
    }

    char hrp[] = "kaspa";

    size_t compressed_pub_size = 32;
    uint8_t compressed_public_key[32] = {0};
    int version = 0;  // 0 - to generate address for schnorr. 1 - to generate address for ECDSA

    // Create the relevant compressed public key
    // For schnorr, compressed public key we care about is the X coordinate
    memmove(compressed_public_key, public_key, compressed_pub_size);

    // First part of the address is "kaspa:"
    memmove(address, hrp, sizeof(hrp));
    address[5] = ':';

    cashaddr_encode(compressed_public_key, compressed_pub_size, address + 6, ADDRESS_LEN, version);

    memmove(out, address, ADDRESS_LEN);

    return true;
}