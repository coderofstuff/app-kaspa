#ifdef HAVE_SWAP
#include "swap.h"
#include "buffer.h"
#include "bip32.h"
#include "crypto_helpers.h"
#include "cx.h"
#include "os.h"

#include "types.h"
#include "format.h"
#include "address.h"
#include "../transaction/types.h"  // SCHNORR_ADDRESS_LEN, ECDSA_ADDRESS_LEN

#include <string.h>

/* Check that the address used to receive funds is owned by the device
 * check_address_parameters_t is defined in C SDK as:
 * struct {
 *   // IN
 *   uint8_t *coin_configuration;
 *   uint8_t  coin_configuration_length;
 *   // serialized path, segwit, version prefix, hash used, dictionary etc.
 *   // fields and serialization format depends on specific coin app
 *   uint8_t *address_parameters;
 *   uint8_t  address_parameters_length;
 *   char    *address_to_check;
 *   char    *extra_id_to_check;
 *   // OUT
 *   int result;
 * } check_address_parameters_t;
 */

static int read_and_validate_bip32_path(const uint8_t *address_parameters,
                                        uint8_t address_parameters_length,
                                        uint32_t *bip32_path,
                                        uint8_t *bip32_path_len) {
    if (address_parameters == NULL) {
        PRINTF("derivation path expected\n");
        return 0;
    }

    buffer_t buf = {.ptr = address_parameters, .size = address_parameters_length, .offset = 0};

    buffer_read_u8(&buf, bip32_path_len);
    buffer_read_bip32_path(&buf, bip32_path, (size_t) *bip32_path_len);

    // Display the BIP32 path immediately after reading
    PRINTF("BIP32 Path: ");
    for (int i = 0; i < *bip32_path_len; i++) {
        PRINTF("%d", bip32_path[i] & 0x7FFFFFFF);
        if (bip32_path[i] & 0x80000000) {
            PRINTF("'");
        }
        if (i < *bip32_path_len - 1) {
            PRINTF("/");
        }
    }
    PRINTF("\n");

    // Now perform validation checks
    if (*bip32_path_len < 2 || *bip32_path_len > KASPA_MAX_BIP32_PATH_LEN) {
        PRINTF("BIP32 path length must be in range [2,5]. Invalid bip32 path length %d\n",
               *bip32_path_len);
        return 0;
    }

    // Validate if bip32_path is bip32_path[0] = 0x8000002C and bip32_path[1] = 0x8001b207
    if (bip32_path[0] != 0x8000002C || bip32_path[1] != 0x8001b207) {
        PRINTF("Invalid bip32 path\n");
        return 0;
    }

    return 1;
}

void swap_handle_check_address(check_address_parameters_t *params) {
    PRINTF("Inside swap_handle_check_address\n");
    params->result = 0;

    if (params->address_parameters == NULL) {
        PRINTF("derivation path expected\n");
        return;
    }
    PRINTF("address_parameters %.*H\n",
           params->address_parameters_length,
           params->address_parameters);

    if (params->address_to_check == NULL) {
        PRINTF("Address to check expected\n");
        return;
    }

    // The address_to_check here is the refund address. We guarantee that it is something we
    // can derived from the bip32 path.
    PRINTF("Address to check %s\n", params->address_to_check);
    if (strlen(params->address_to_check) != SCHNORR_ADDRESS_LEN) {
        PRINTF("Address to check expected length %d or %d, not %d\n",
               SCHNORR_ADDRESS_LEN,
               ECDSA_ADDRESS_LEN,
               strlen(params->address_to_check));
        return;
    }

    uint8_t bip32_path_len;
    uint32_t bip32_path[MAX_BIP32_PATH];
    pubkey_ctx_t pk_info = {0};

    if (!read_and_validate_bip32_path(params->address_parameters,
                                      params->address_parameters_length,
                                      bip32_path,
                                      &bip32_path_len)) {
        return;
    }

    uint8_t raw_pubkey[65] = {0};
    cx_err_t ret = bip32_derive_get_pubkey_256(CX_CURVE_256K1,
                                               bip32_path,
                                               bip32_path_len,
                                               raw_pubkey + 1,
                                               pk_info.chain_code,
                                               CX_SHA512);
    memmove(pk_info.raw_public_key, raw_pubkey + 1, 64);

    if (ret != CX_OK) {
        PRINTF("Failed to derive public key\n");
        return;
    }

    uint8_t address[SCHNORR_ADDRESS_LEN] = {0};
    if (!address_from_pubkey(pk_info.raw_public_key, SCHNORR, address, sizeof(address))) {
        PRINTF("Failed to convert public key to address\n");
        return;
    }

    char derived_address[SCHNORR_ADDRESS_LEN + 1];
    memset(derived_address, 0, sizeof(derived_address));
    snprintf(derived_address, sizeof(derived_address), "%.*s", sizeof(address), address);
    PRINTF("Derived address %s\n", address);

    PRINTF("Checked address %s\n", params->address_to_check);

    if (strncmp(derived_address, params->address_to_check, sizeof(derived_address)) != 0) {
        PRINTF("Addresses do not match\n");
    } else {
        PRINTF("Addresses match\n");
        params->result = 1;
    }
}
#endif  // HAVE_SWAP
