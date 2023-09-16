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
#pragma once

#include "os.h"
#include "macros.h"

/**
 * Length of public key.
 */
#define PUBKEY_LEN (MEMBER_SIZE(pubkey_ctx_t, raw_public_key))
/**
 * Length of chain code.
 */
#define CHAINCODE_LEN (MEMBER_SIZE(pubkey_ctx_t, chain_code))

/**
 * Helper to send APDU response with public key and chain code.
 *
 * response = PUBKEY_LEN (1) ||
 *            G_context.pk_info.public_key (PUBKEY_LEN) ||
 *            CHAINCODE_LEN (1) ||
 *            G_context.pk_info.chain_code (CHAINCODE_LEN)
 *
 * @return zero or positive integer if success, -1 otherwise.
 *
 */
int helper_send_response_pubkey(void);

/**
 * Helper to send APDU response with signature and v (parity of
 * y-coordinate of R).
 *
 * response = MAX_DER_SIG_LEN (1) ||
 *            G_context.tx_info.signature (MAX_DER_SIG_LEN)
 *
 * @return zero or positive integer if success, -1 otherwise.
 *
 */
int helper_send_response_sig(void);

/**
 * Helper to send APDU response with personal message signature
 * response = MAX_DER_SIG_LEN (1) ||
 *            G_context.tx_info.signature (MAX_DER_SIG_LEN)
 *
 * @return zero or positive integer if success, -1 otherwise.
 *
 */
int helper_send_response_personal_message_sig(void);
