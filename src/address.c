#include <stdint.h>   // uint*_t
#include <stddef.h>   // size_t
#include <stdbool.h>  // bool
#include <string.h>   // memmove

#include "address.h"
#include "./import/cashaddr.h"

#include "transaction/types.h"

size_t compress_public_key(const uint8_t public_key[static 64],
                           address_type_e address_type,
                           uint8_t *out) {
    size_t compressed_pub_size = 0;
    if (address_type == SCHNORR) {
        compressed_pub_size = 32;
        memmove(out, public_key, 32);
    } else {
        compressed_pub_size = 33;
        // If Y coord is even, first byte is 0x02. if odd then 0x03
        out[0] = public_key[63] % 2 == 0 ? 0x02 : 0x03;
        // We copy starting from the 2nd byte
        memmove(out + 1, public_key, 32);
    }

    return compressed_pub_size;
}

bool address_from_pubkey(const uint8_t public_key[static 64],
                         address_type_e address_type,
                         uint8_t *out,
                         size_t out_len) {
    uint8_t address[80] = {0};

    size_t address_len = SCHNORR_ADDRESS_LEN;
    int version = 0;

    if (address_type == ECDSA) {
        address_len = ECDSA_ADDRESS_LEN;
        version = 1;
    }

    if (out_len < address_len) {
        return false;
    }

    char hrp[] = "kaspa";

    // Choose the bigger length for public key container
    uint8_t compressed_public_key[33] = {0};

    // Create the relevant compressed public key
    // For schnorr, compressed public key we care about is the X coordinate
    // For ecdsa, compress public key is 1 byte (y-coord: 0x02 if even, 0x03 if odd) then X
    // coordinate
    size_t compressed_pub_size =
        compress_public_key(public_key, address_type, compressed_public_key);

    // First part of the address is "kaspa:"
    memmove(address, hrp, sizeof(hrp));
    address[5] = ':';

    cashaddr_encode(compressed_public_key, compressed_pub_size, address + 6, address_len, version);

    memmove(out, address, address_len);

    return true;
}