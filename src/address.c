#include <stdint.h>   // uint*_t
#include <stddef.h>   // size_t
#include <stdbool.h>  // bool
#include <string.h>   // memmove

#include "os.h"
#include "cx.h"

#include "address.h"

#include "transaction/types.h"

bool address_from_pubkey(const uint8_t public_key[static 64], uint8_t *out, size_t out_len) {
    uint8_t address[32] = {0};
    cx_sha3_t keccak256;

    if (out_len < ADDRESS_LEN) {
        return false;
    }

    cx_keccak_init(&keccak256, 256);
    cx_hash((cx_hash_t *) &keccak256, CX_LAST, public_key, 64, address, sizeof(address));

    memmove(out, address + sizeof(address) - ADDRESS_LEN, ADDRESS_LEN);

    return true;
}
