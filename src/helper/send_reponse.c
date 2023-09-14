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
#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t
#include <string.h>  // memmove

#include "send_response.h"
#include "../constants.h"
#include "../globals.h"
#include "../sw.h"
#include "buffer.h"

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

    return io_send_response_pointer(resp, offset, SW_OK);
}

int helper_send_response_sig() {
    uint8_t resp[3 + MAX_DER_SIG_LEN + 34] = {0};
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
    // len(sighash) -> 1 byte
    resp[offset++] = sizeof(G_context.tx_info.sighash);
    // sighash -> 32 bytes
    memmove(resp + offset, G_context.tx_info.sighash, sizeof(G_context.tx_info.sighash));
    offset += sizeof(G_context.tx_info.sighash);

    return io_send_response_pointer(resp, offset, SW_OK);
}
