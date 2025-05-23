#ifdef HAVE_SWAP
#include "swap.h"
#include "swap_error_code_helpers.h"
#include "os.h"
#include "format.h"

#include "sw.h"
#include "globals.h"
#include "types.h"
#include "handle_swap.h"

#include <stdint.h>

typedef struct swap_validated_s {
    bool initialized;
    uint64_t amount;
    uint64_t fee;
    char recipient[ECDSA_ADDRESS_LEN + 1];  // A standard address can be max 69 in len
} swap_validated_t;

/* Global variable used to store swap validation status */
static swap_validated_t G_swap_validated;

static const char *charset = "qpzry9x8gf2tvdw0s3jn54khce6mua7l";

static bool is_valid_charset_char(char c) {
    for (size_t i = 0; i < strlen(charset); i++) {
        if (c == charset[i]) {
            return true;
        }
    }
    return false;
}

static bool is_valid_address_charset(const char *addr, bool has_prefix) {
    const char *addr_to_check = has_prefix ? addr + 6 : addr;
    size_t len = strlen(addr_to_check);

    for (size_t i = 0; i < len; i++) {
        if (!is_valid_charset_char(addr_to_check[i])) {
            return false;
        }
    }
    return true;
}

bool swap_copy_transaction_parameters(create_transaction_parameters_t *params) {
    PRINTF("Inside swap_copy_transaction_parameters %s\n", params->destination_address);

    if (params->destination_address == NULL) {
        PRINTF("Destination address expected\n");
        return false;
    }

    // Destination address is "normalized" by normalize_address from app-exchange,
    // meaning if an address has a ':', it only takes the characters after the ':'.
    // So we check also if the address has "kaspa:" prefix or not.
    // Only minimal checks are necessary here since later when we actually convert the SPK
    // if the saved address here is not valid, it will never match.
    int len = strlen(params->destination_address);
    bool has_prefix = (strncmp(params->destination_address, "kaspa:", 6) == 0);
    bool has_valid_len_when_has_prefix =
        has_prefix && (len == ECDSA_ADDRESS_LEN || len == SCHNORR_ADDRESS_LEN);
    bool has_valid_len_when_prefix_removed =
        !has_prefix && (len + 6 == ECDSA_ADDRESS_LEN || len + 6 == SCHNORR_ADDRESS_LEN);
    if (!has_valid_len_when_has_prefix && !has_valid_len_when_prefix_removed) {
        PRINTF("Destination address wrong length of %d\n", strlen(params->destination_address));
        return false;
    }

    // Check charset validity only if length checks passed
    if (!is_valid_address_charset(params->destination_address, has_prefix)) {
        PRINTF("Destination address contains invalid characters\n");
        return false;
    }

    if (params->amount == NULL) {
        PRINTF("Amount expected\n");
        return false;
    }

    // first copy parameters to stack, and then to global data.
    // We need this "trick" as the input data position can overlap with app globals
    // and also because we want to memset the whole bss segment as it is not done
    // when an app is called as a lib.
    // This is necessary as many part of the code expect bss variables to
    // initialized at 0.
    swap_validated_t swap_validated;
    memset(&swap_validated, 0, sizeof(swap_validated));

    // Save recipient as is. Add "kaspa:" prefix if missing
    // This logic depends on the normalize_address function in app-exchange
    // which removes the "kaspa:" prefix part due to the ':'.
    // See this line:
    // https://github.com/LedgerHQ/app-exchange/blob/fc2f1d6578db5fe778c6343a359b38d7fb730cd2/src/process_transaction.c#L288
    // The implementation here is robust against changes to this normalize_address
    if (!has_prefix) {
        strncpy(swap_validated.recipient, "kaspa:", sizeof(swap_validated.recipient));
        strncpy(swap_validated.recipient + 6,
                params->destination_address,
                sizeof(swap_validated.recipient) - 6);
    } else {
        strncpy(swap_validated.recipient,
                params->destination_address,
                sizeof(swap_validated.recipient));
    }

    // Save amount
    if (!swap_str_to_u64(params->amount, params->amount_length, &swap_validated.amount)) {
        PRINTF("Failed to convert amount to uint64_t\n");
        return false;
    }

    // Save fee
    if (!swap_str_to_u64(params->fee_amount, params->fee_amount_length, &swap_validated.fee)) {
        PRINTF("Failed to convert fee to uint64_t\n");
        return false;
    }

    swap_validated.initialized = true;

    // Full reset the global variables
    os_explicit_zero_BSS_segment();

    // Commit from stack to global data, params becomes tainted but we won't access it anymore
    memcpy(&G_swap_validated, &swap_validated, sizeof(swap_validated));

    PRINTF("Out of swap_copy_transaction_parameters\n");

    return true;
}

/* Check if the Tx to sign have the same parameters as the ones previously validated */
bool swap_check_validity(uint64_t amount, uint64_t fee, const uint8_t *destination) {
    PRINTF("Inside swap_check_validity\n");

    if (!G_swap_validated.initialized) {
        PRINTF("Swap structure is not initialized\n");
        send_swap_error_simple(SW_SWAP_FAIL, SWAP_EC_ERROR_GENERIC, SWAP_ERROR_CODE);
        // unreachable
        os_sched_exit(0);
    }

    if (G_swap_validated.amount != amount) {
        PRINTF("Amount does not match, promised %lld, received %lld\n",
               G_swap_validated.amount,
               amount);
        send_swap_error_simple(SW_SWAP_FAIL, SWAP_EC_ERROR_WRONG_AMOUNT, SWAP_ERROR_CODE);
        // unreachable
        os_sched_exit(0);
    } else {
        PRINTF("Amounts match \n");
    }

    if (G_swap_validated.fee != fee) {
        PRINTF("Fee does not match, promised %lld, received %lld\n", G_swap_validated.fee, fee);
        send_swap_error_simple(SW_SWAP_FAIL, SWAP_EC_ERROR_WRONG_FEES, SWAP_ERROR_CODE);
        // unreachable
        os_sched_exit(0);
    } else {
        PRINTF("Fees match \n");
    }

    if (strcmp(G_swap_validated.recipient, (const char *) destination) != 0) {
        PRINTF("Destination does not match\n");
        PRINTF("Validated: %s\n", G_swap_validated.recipient);
        PRINTF("Received: %s \n", destination);
        send_swap_error_simple(SW_SWAP_FAIL, SWAP_EC_ERROR_WRONG_DESTINATION, SWAP_ERROR_CODE);
        // unreachable
        os_sched_exit(0);
    } else {
        PRINTF("Destination is valid\n");
    }
    return true;
}
#endif  // HAVE_SWAP
