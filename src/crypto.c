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
#include <string.h>   // memset, explicit_bzero
#include <stdbool.h>  // bool

#include "crypto_helpers.h"
#include "crypto.h"

#include "globals.h"

#include "sighash.h"
#include "personal_message.h"

bool crypto_validate_public_key(const uint32_t *bip32_path,
                                uint8_t bip32_path_len,
                                uint8_t compressed_public_key[static 32]) {
    uint8_t raw_pubkey[65] = {0};
    uint8_t chain_code[32] = {0};

    int error = bip32_derive_get_pubkey_256(CX_CURVE_256K1,
                                            bip32_path,
                                            bip32_path_len,
                                            raw_pubkey,
                                            chain_code,
                                            CX_SHA512);

    if (error != CX_OK) {
        return false;
    }

    PRINTF("==> Generated Data: %.*H\n", 32, raw_pubkey + 1);
    PRINTF("==> Passed Data   : %.*H\n", 32, compressed_public_key);
    return memcmp(raw_pubkey + 1, compressed_public_key, 32) == 0;
}

int crypto_sign_transaction(void) {
    cx_ecfp_private_key_t private_key = {0};
    cx_ecfp_public_key_t public_key = {0};
    uint8_t chain_code[32] = {0};

    transaction_input_t *txin =
        &G_context.tx_info.transaction.tx_inputs[G_context.tx_info.signing_input_index];

    // 44'/111111'/account'/ address_type / address_index
    G_context.bip32_path[0] = 0x8000002C;
    G_context.bip32_path[1] = 0x8001b207;
    G_context.bip32_path[2] = G_context.tx_info.transaction.account;
    G_context.bip32_path[3] = (uint32_t) (txin->address_type);
    G_context.bip32_path[4] = txin->address_index;

    G_context.bip32_path_len = 5;

    int error = bip32_derive_init_privkey_256(CX_CURVE_256K1,
                                              G_context.bip32_path,
                                              G_context.bip32_path_len,
                                              &private_key,
                                              chain_code);
    if (error != CX_OK) {
        return error;
    }

    BEGIN_TRY {
        TRY {
            // Clear the sighash and signature before trying to use it:
            memset(G_context.tx_info.sighash, 0, sizeof(G_context.tx_info.sighash));
            memset(G_context.tx_info.signature, 0, sizeof(G_context.tx_info.signature));

            error = cx_ecfp_generate_pair_no_throw(CX_CURVE_256K1, &public_key, &private_key, 1);
            if (error != CX_OK) {
                return error;
            }

            if (!calc_sighash(&G_context.tx_info.transaction,
                              txin,
                              public_key.W + 1,
                              G_context.tx_info.sighash,
                              sizeof(G_context.tx_info.sighash))) {
                return -1;
            }

            size_t sig_len = sizeof(G_context.tx_info.signature);
            error = cx_ecschnorr_sign_no_throw(&private_key,
                                               CX_ECSCHNORR_BIP0340 | CX_RND_TRNG,
                                               CX_SHA256,
                                               G_context.tx_info.sighash,
                                               sizeof(G_context.tx_info.sighash),
                                               G_context.tx_info.signature,
                                               &sig_len);
            if (error != CX_OK) {
                PRINTF("Signature: %.*H\n", 64, G_context.tx_info.signature);
            }
        }
        FINALLY {
            explicit_bzero(&public_key, sizeof(public_key));
            explicit_bzero(&private_key, sizeof(private_key));
        }
    }
    END_TRY;

    return error;
}

int crypto_sign_personal_message(void) {
    if (!hash_personal_message(G_context.msg_info.message,
                               G_context.msg_info.message_len,
                               G_context.msg_info.message_hash,
                               sizeof(G_context.msg_info.message_hash))) {
        return -1;
    }

    cx_ecfp_private_key_t private_key = {0};
    uint8_t chain_code[32] = {0};

    int error = bip32_derive_init_privkey_256(CX_CURVE_256K1,
                                              G_context.bip32_path,
                                              G_context.bip32_path_len,
                                              &private_key,
                                              chain_code);

    BEGIN_TRY {
        TRY {
            size_t sig_len = sizeof(G_context.tx_info.signature);
            error = cx_ecschnorr_sign_no_throw(&private_key,
                                               CX_ECSCHNORR_BIP0340 | CX_RND_TRNG,
                                               CX_SHA256,
                                               G_context.msg_info.message_hash,
                                               sizeof(G_context.msg_info.message_hash),
                                               G_context.msg_info.signature,
                                               &sig_len);
        }
        FINALLY {
            explicit_bzero(&private_key, sizeof(private_key));
        }
    }
    END_TRY;

    return error;
}
