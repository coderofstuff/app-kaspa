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
    uint8_t resp[1 + MAX_DER_SIG_LEN + 1] = {0};
    size_t offset = 0;

    resp[offset++] = G_context.tx_info.signature_len;
    memmove(resp + offset, G_context.tx_info.signature, G_context.tx_info.signature_len);
    offset += G_context.tx_info.signature_len;
    resp[offset++] = (uint8_t) G_context.tx_info.v;

    return io_send_response(&(const buffer_t){.ptr = resp, .size = offset, .offset = 0}, SW_OK);
}
