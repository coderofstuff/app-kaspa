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
#ifdef HAVE_NBGL

#pragma GCC diagnostic ignored "-Wformat-invalid-specifier"  // snprintf
#pragma GCC diagnostic ignored "-Wformat-extra-args"         // snprintf

#include <stdbool.h>  // bool
#include <string.h>   // memset

#include "os.h"
#include "glyphs.h"
#include "nbgl_use_case.h"

#include "display.h"
#include "constants.h"
#include "../globals.h"
#include "io.h"
#include "../sw.h"
#include "../address.h"
#include "action/validate.h"
#include "../types.h"
#include "../transaction/types.h"
#include "bip32.h"
#include "../common/format_local.h"
#include "format.h"
#include "../menu.h"

static char g_message[MAX_MESSAGE_LEN];
static char g_bip32_path[60];

static nbgl_layoutTagValue_t pairs[2];
static nbgl_layoutTagValueList_t pairList;
static nbgl_pageInfoLongPress_t infoLongPress;

static void confirm_message_rejection(void) {
    // display a status page and go back to main
    validate_message(false);
    nbgl_useCaseStatus("Message signing\ncancelled", false, ui_menu_main);
}

static void ask_message_rejection_confirmation(void) {
    // display a choice to confirm/cancel rejection
    nbgl_useCaseConfirm("Reject message?",
                        NULL,
                        "Yes, Reject",
                        "Go back to message",
                        confirm_message_rejection);
}

static void confirm_message_approval(void) {
    // display a success status page and go back to main
    validate_message(true);
    nbgl_useCaseStatus("MESSAGE\nSIGNED", true, ui_menu_main);
}

static void review_message_choice(bool confirm) {
    if (confirm) {
        confirm_message_approval();
    } else {
        ask_message_rejection_confirmation();
    }
}

static void continue_message_review(void) {
    // Fill pairs
    pairs[0].item = "BIP32 Path";
    pairs[0].value = g_bip32_path;
    pairs[1].item = "Message";
    pairs[1].value = g_message;

    // Setup list
    pairList.nbMaxLinesForValue = 0;
    pairList.nbPairs = 2;
    pairList.pairs = pairs;

    // Info long press
    infoLongPress.icon = &C_stax_app_kaspa_64px;
    infoLongPress.text = "Sign message?";
    infoLongPress.longPressText = "Hold to sign";

    nbgl_useCaseStaticReview(&pairList, &infoLongPress, "Reject message", review_message_choice);
}

int ui_display_message() {
    if (G_context.req_type != CONFIRM_MESSAGE || G_context.state != STATE_NONE) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    memset(g_bip32_path, 0, sizeof(g_bip32_path));
    if (!bip32_path_format(G_context.bip32_path,
                           G_context.bip32_path_len,
                           g_bip32_path,
                           sizeof(g_bip32_path))) {
        return io_send_sw(SW_DISPLAY_BIP32_PATH_FAIL);
    }

    memset(g_message, 0, sizeof(g_message));
    format_message_to_sign(g_message,
                           sizeof(g_message),
                           (char *) G_context.msg_info.message,
                           G_context.msg_info.message_len);

    nbgl_useCaseReviewStart(&C_stax_app_kaspa_64px,
                            "Review Message",
                            NULL,
                            "Reject message",
                            continue_message_review,
                            ask_message_rejection_confirmation);
    return 0;
}

#endif
