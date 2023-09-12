
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

#include <stdint.h>  // uint*_t

#include "cx.h"

/**
 * Sign message hash in global context.
 *
 * @see G_context.bip32_path,
 * G_context.tx_info.signature.
 *
 * @return 0 on success, error number otherwise.
 *
 */
int crypto_sign_transaction(void);

/**
 * Checks if the compressed public key matches the
 * one generated from the bip32 path
 *
 * @param[in]  bip32_path
 *   Pointer to buffer with BIP32 path.
 * @param[in]  bip32_path_len
 *   Number of path in BIP32 path.
 * @param[in]  compressed_public_key
 *   The 32-byte compressed public key to compare to
 *
 * @return 0 on success, error number otherwise.
 *
 */
bool crypto_validate_public_key(const uint32_t *bip32_path,
                                uint8_t bip32_path_len,
                                uint8_t compressed_public_key[static 32]);

/**
 * Sign personal message hash in global context.
 *
 * @see G_context.bip32_path,
 * G_context.msg_info.signature.
 *
 * @return 0 on success, error number otherwise.
 *
 */
int crypto_sign_personal_message(void);