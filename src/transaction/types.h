#pragma once

#include <stddef.h>   // size_t
#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool

#define SCHNORR_ADDRESS_LEN 67
#define ECDSA_ADDRESS_LEN   69

typedef enum {
    PARSING_OK = 1,
    VERSION_PARSING_ERROR = -1,
    OUTPUTS_LENGTH_PARSING_ERROR = -2,
    INPUTS_LENGTH_PARSING_ERROR = -3,
    HEADER_PARSING_ERROR = -4,
    INPUT_PARSING_ERROR = -20,
    INPUT_VALUE_PARSING_ERROR = -21,
    INPUT_TX_ID_PARSING_ERROR = -22,
    INPUT_ADDRESS_TYPE_PARSING_ERROR = -23,
    INPUT_ADDRESS_INDEX_PARSING_ERROR = -24,
    INPUT_INDEX_PARSING_ERROR = -25,
    OUTPUT_PARSING_ERROR = -30,
    OUTPUT_VALUE_PARSING_ERROR = -31,
    OUTPUT_SCRIPT_PUBKEY_PARSING_ERROR = -32
} parser_status_e;

typedef enum { SIGHASH_PARSING_OK = 1 } sighash_status_e;

typedef enum {
    RECEIVE = 0,  // For receive addresses
    CHANGE = 1    // For change addresses
} derive_type_e;

/**
 * Enumeration of op codes
 */
typedef enum {
    OP_CHECKSIG = 0xac,      // Used for SCHNORR output
    OP_CHECKSIGECDSA = 0xab  // Used for ECDSA output
} op_code_e;

typedef struct {
    uint8_t address_type;
    uint32_t address_index;
    uint64_t sequence;
    uint64_t value;
    uint8_t tx_id[32];
    uint8_t index;
} transaction_input_t;

typedef struct {
    uint64_t value;
    uint8_t script_public_key[35];  // In hex: 20 + public_key_hex + ac (34/35 bytes total)
} transaction_output_t;

typedef struct {
    // For signature purposes:
    // Based on: https://kaspa-mdbook.aspectron.com/transactions/constraints/size.html
    uint16_t version;
    size_t tx_input_len;  // check
    size_t tx_output_len;

    transaction_output_t tx_outputs[2];
    transaction_input_t tx_inputs[MAX_INPUT_COUNT];  // array of inputs

    // uint64_t lock_time;      // Don't support this yet
    // uint8_t* subnetwork_id;  // Don't support this yet
    // uint64_t gas;            // Don't support this yet
    // uin64_t  payload_len;     // Don't support this yet
    // uint8_t* payload;        // Don't support this yet
} transaction_t;
