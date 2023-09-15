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
#include <stdbool.h>  // bool

#include "validate.h"
#include "../menu.h"
#include "../../sw.h"
#include "../../crypto.h"
#include "../../globals.h"
#include "../../helper/send_response.h"

void validate_pubkey(bool choice) {
    if (choice) {
        helper_send_response_pubkey();
    } else {
        io_send_sw(SW_DENY);
    }
}

void validate_transaction(bool choice) {
    if (choice) {
        G_context.state = STATE_APPROVED;

        int error = crypto_sign_transaction();
        if (error != 0) {
            G_context.state = STATE_NONE;
            io_send_sw(error);
        } else {
            helper_send_response_sig();
            G_context.tx_info.signing_input_index++;
        }
    } else {
        G_context.state = STATE_NONE;
        io_send_sw(SW_DENY);
    }
}

void validate_message(bool choice) {
    if (choice) {
        G_context.state = STATE_APPROVED;

        int error = crypto_sign_personal_message();
        if (error != 0) {
            G_context.state = STATE_NONE;
            io_send_sw(error);
        } else {
            helper_send_response_personal_message_sig();
        }
    } else {
        G_context.state = STATE_NONE;
        io_send_sw(SW_DENY);
    }
}