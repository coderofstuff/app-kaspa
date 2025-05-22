/*****************************************************************************
 * MIT License
 *
 * Copyright (c) 2023 coderofstuff
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/
#include <stdint.h>   // uint*_t
#include <stddef.h>   // size_t
#include <stdbool.h>  // bool
#include <string.h>   // memmove

#include "address.h"
#include "./import/cashaddr.h"

#include "transaction/types.h"

size_t compress_public_key(const uint8_t public_key[static 64],
                           address_type_e address_type,
                           uint8_t *out,
                           size_t out_len) {
    size_t compressed_pub_size = 0;
    if (address_type == SCHNORR || address_type == P2SH) {
        compressed_pub_size = 32;
        if (out_len < 32) {
            return 0;
        }
        memmove(out, public_key, 32);
    } else if (address_type == ECDSA) {
        if (out_len < 33) {
            return 0;
        }
        compressed_pub_size = 33;
        // If Y coord is even, first byte is 0x02. if odd then 0x03
        out[0] = public_key[63] % 2 == 0 ? 0x02 : 0x03;
        // We copy starting from the 2nd byte
        memmove(out + 1, public_key, 32);
    } else {
        return 0;
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
    } else if (address_type == P2SH) {
        address_len = SCHNORR_ADDRESS_LEN;
        version = 8;
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
    size_t compressed_pub_size = compress_public_key(public_key,
                                                     address_type,
                                                     compressed_public_key,
                                                     sizeof(compressed_public_key));

    if (compressed_pub_size == 0) {
        return false;
    }

    // First part of the address is "kaspa:"
    memmove(address, hrp, sizeof(hrp));
    address[5] = ':';

    if (cashaddr_encode(compressed_public_key,
                        compressed_pub_size,
                        address + 6,
                        address_len,
                        version) == 0) {
        return false;
    }

    memmove(out, address, address_len);

    return true;
}
