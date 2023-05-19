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
#include <stdbool.h>  // bool
#include <stddef.h>   // size_t
#include <string.h>   // memset, explicit_bzero

#include "os.h"
#include "cx.h"

#include "get_public_key.h"
#include "../globals.h"
#include "../types.h"
#include "../io.h"
#include "../sw.h"
#include "../crypto.h"
#include "../common/buffer.h"
#include "../ui/display.h"
#include "../helper/send_response.h"

int handler_get_public_key(buffer_t *cdata, bool display) {
    explicit_bzero(&G_context, sizeof(G_context));
    G_context.req_type = CONFIRM_ADDRESS;
    G_context.state = STATE_NONE;

    cx_ecfp_private_key_t private_key = {0};
    cx_ecfp_public_key_t public_key = {0};

    if (!buffer_read_u8(cdata, &G_context.bip32_path_len) ||
        !buffer_read_bip32_path(cdata, G_context.bip32_path, (size_t) G_context.bip32_path_len)) {
        return io_send_sw(SW_WRONG_DATA_LENGTH);
    }

    if (G_context.bip32_path_len != 5) {
        return io_send_sw(SW_WRONG_BIP32_PATH_LEN);
    }

    if (G_context.bip32_path[0] != (uint32_t) 0x8000002c) {
        return io_send_sw(SW_WRONG_BIP32_PURPOSE);
    }

    if (G_context.bip32_path[1] != (uint32_t) 0x8001b207) {
        return io_send_sw(SW_WRONG_BIP32_COIN_TYPE);
    }

    if (G_context.bip32_path[3] != (uint32_t) RECEIVE &&
        G_context.bip32_path[3] != (uint32_t) CHANGE) {
        return io_send_sw(SW_WRONG_BIP32_TYPE);
    }

    // derive private key according to BIP32 path
    int error = crypto_derive_private_key(&private_key,
                                          G_context.pk_info.chain_code,
                                          G_context.bip32_path,
                                          G_context.bip32_path_len);
    if (error != 0) {
        return io_send_sw(error);
    }
    // generate corresponding public key
    crypto_init_public_key(&private_key, &public_key, G_context.pk_info.raw_public_key);
    // reset private key
    explicit_bzero(&private_key, sizeof(private_key));

    if (display) {
        return ui_display_address();
    }

    return helper_send_response_pubkey();
}
