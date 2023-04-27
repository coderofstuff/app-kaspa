#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t
#include <string.h>  // memmove

#include "send_response.h"
#include "../constants.h"
#include "../globals.h"
#include "../sw.h"
#include "common/buffer.h"

int helper_send_response_pubkey() {
    uint8_t resp[1 + 1 + PUBKEY_LEN + 1 + CHAINCODE_LEN] = {0};
    size_t offset = 0;

    resp[offset++] = PUBKEY_LEN + 1;
    resp[offset++] = 0x04;
    memmove(resp + offset, G_context.pk_info.raw_public_key, PUBKEY_LEN);
    offset += PUBKEY_LEN;
    resp[offset++] = CHAINCODE_LEN;
    memmove(resp + offset, G_context.pk_info.chain_code, CHAINCODE_LEN);
    offset += CHAINCODE_LEN;

    return io_send_response(&(const buffer_t){.ptr = resp, .size = offset, .offset = 0}, SW_OK);
}

int helper_send_response_sig() {
    uint8_t resp[3 + MAX_DER_SIG_LEN + 1] = {0};
    size_t offset = 0;

    // has_more -> 1 byte
    resp[offset++] =
        G_context.tx_info.transaction.tx_input_len - G_context.tx_info.signing_input_index - 1;
    // input_index -> 1 byte
    resp[offset++] = G_context.tx_info.signing_input_index;
    // len(sig) -> 1 byte
    resp[offset++] = MAX_DER_SIG_LEN;
    // sig -> 64 bytes
    memmove(resp + offset, G_context.tx_info.signature, MAX_DER_SIG_LEN);
    offset += MAX_DER_SIG_LEN;

    return io_send_response(&(const buffer_t){.ptr = resp, .size = offset, .offset = 0}, SW_OK);
}
