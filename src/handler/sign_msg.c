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
#include <stdint.h>  // uint*_t
#include <string.h>  // memset, explicit_bzero

#include "types.h"
#include "buffer.h"
#include "./globals.h"
#include "./sign_msg.h"
#include "../sw.h"
#include "../ui/display.h"
#include "../helper/send_response.h"

/**
 * Handler for SIGN_MESSAGE command. If successfully parse BIP32 path
 * and message, sign the message and send APDU response.
 *
 * @see G_context.bip32_path, G_context.msg_info
 *
 * @param[in,out] cdata
 *   Command data with BIP32 path and raw message.
 *
 * @return zero or positive integer if success, negative integer otherwise.
 *
 */
int handler_sign_msg(buffer_t *cdata) {
    explicit_bzero(&G_context, sizeof(G_context));
    G_context.req_type = CONFIRM_MESSAGE;
    G_context.state = STATE_NONE;

    if (!buffer_read_u8(cdata, &G_context.msg_info.address_type)) {
        return io_send_sw(SW_MESSAGE_ADDRESS_TYPE_FAIL);
    }

    if (G_context.msg_info.address_type != 0 && G_context.msg_info.address_type != 1) {
        return io_send_sw(SW_MESSAGE_ADDRESS_TYPE_FAIL);
    }

    if (!buffer_read_u32(cdata, &G_context.msg_info.address_index, BE)) {
        return io_send_sw(SW_MESSAGE_ADDRESS_TYPE_FAIL);
    }

    if (!buffer_read_u32(cdata, &G_context.msg_info.account, BE)) {
        return io_send_sw(SW_MESSAGE_ADDRESS_TYPE_FAIL);
    }

    if (G_context.msg_info.account < 0x80000000) {
        return io_send_sw(SW_MESSAGE_ADDRESS_TYPE_FAIL);
    }

    uint8_t message_len = 0;
    if (!buffer_read_u8(cdata, &message_len)) {
        return io_send_sw(SW_MESSAGE_LEN_PARSING_FAIL);
    }

    if (message_len == 0) {
        return io_send_sw(SW_MESSAGE_TOO_SHORT);
    }

    if (message_len > MAX_MESSAGE_LEN) {
        return io_send_sw(SW_MESSAGE_TOO_LONG);
    }

    G_context.msg_info.message_len = (size_t) message_len;

    if (!buffer_can_read(cdata, G_context.msg_info.message_len)) {
        return io_send_sw(SW_MESSAGE_PARSING_FAIL);
    }

    memcpy(G_context.msg_info.message, cdata->ptr + cdata->offset, G_context.msg_info.message_len);

    if (!buffer_seek_cur(cdata, G_context.msg_info.message_len)) {
        return io_send_sw(SW_MESSAGE_UNEXPECTED);
    }

    G_context.bip32_path[0] = 0x8000002C;
    G_context.bip32_path[1] = 0x8001b207;
    G_context.bip32_path[2] = G_context.msg_info.account;
    G_context.bip32_path[3] = (uint32_t) (G_context.msg_info.address_type);
    G_context.bip32_path[4] = G_context.msg_info.address_index;

    G_context.bip32_path_len = 5;

    return ui_display_message();
}