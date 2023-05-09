#include <stdint.h>   // uint*_t
#include <string.h>   // memset, explicit_bzero
#include <stdbool.h>  // bool

#include "crypto.h"

#include "globals.h"

#include "sighash.h"

int crypto_derive_private_key(cx_ecfp_private_key_t *private_key,
                              uint8_t chain_code[static 32],
                              const uint32_t *bip32_path,
                              uint8_t bip32_path_len) {
    uint8_t raw_private_key[32] = {0};
    int error = 0;

    BEGIN_TRY {
        TRY {
            // derive the seed with bip32_path
            os_perso_derive_node_bip32(CX_CURVE_256K1,
                                       bip32_path,
                                       bip32_path_len,
                                       raw_private_key,
                                       chain_code);
            // new private_key from raw
            cx_ecfp_init_private_key(CX_CURVE_256K1,
                                     raw_private_key,
                                     sizeof(raw_private_key),
                                     private_key);
        }
        CATCH_OTHER(e) {
            error = e;
        }
        FINALLY {
            explicit_bzero(&raw_private_key, sizeof(raw_private_key));
        }
    }
    END_TRY;

    return error;
}

void crypto_init_public_key(cx_ecfp_private_key_t *private_key,
                            cx_ecfp_public_key_t *public_key,
                            uint8_t raw_public_key[static 64]) {
    // generate corresponding public key
    cx_ecfp_generate_pair(CX_CURVE_256K1, public_key, private_key, 1);

    memmove(raw_public_key, public_key->W + 1, 64);
}

int crypto_sign_message(void) {
    cx_ecfp_private_key_t private_key = {0};
    cx_ecfp_public_key_t public_key = {0};
    uint8_t chain_code[32] = {0};
    uint32_t info = 0;

    transaction_input_t *txin =
        &G_context.tx_info.transaction.tx_inputs[G_context.tx_info.signing_input_index];

    // 44'/111111'/0'/ address_type / address_index
    G_context.bip32_path[0] = 0x8000002C;
    G_context.bip32_path[1] = 0x8001b207;
    G_context.bip32_path[2] = 0x80000000;
    G_context.bip32_path[3] = (uint32_t)(txin->address_type);
    G_context.bip32_path[4] = txin->address_index;

    G_context.bip32_path_len = 5;

    // derive private key according to BIP32 path
    int error = crypto_derive_private_key(&private_key,
                                          chain_code,
                                          G_context.bip32_path,
                                          G_context.bip32_path_len);
    if (error != 0) {
        return error;
    }

    BEGIN_TRY {
        TRY {
            // Clear the sighash and signature before trying to use it:
            memset(G_context.tx_info.sighash, 0, sizeof(G_context.tx_info.sighash));
            memset(G_context.tx_info.signature, 0, sizeof(G_context.tx_info.signature));

            cx_ecfp_generate_pair(CX_CURVE_256K1, &public_key, &private_key, 1);
            calc_sighash(&G_context.tx_info.transaction,
                         txin,
                         public_key.W + 1,
                         G_context.tx_info.sighash);
            cx_ecschnorr_sign(&private_key,
                              CX_ECSCHNORR_BIP0340 | CX_RND_TRNG,
                              CX_SHA256,
                              G_context.tx_info.sighash,
                              sizeof(G_context.tx_info.sighash),
                              G_context.tx_info.signature,
                              sizeof(G_context.tx_info.signature),
                              &info);
            PRINTF("Signature: %.*H\n", 64, G_context.tx_info.signature);
        }
        CATCH_OTHER(e) {
            error = e;
        }
        FINALLY {
            explicit_bzero(&public_key, sizeof(public_key));
            explicit_bzero(&private_key, sizeof(private_key));
        }
    }
    END_TRY;

    return error;
}
