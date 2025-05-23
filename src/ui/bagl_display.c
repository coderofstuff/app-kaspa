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
#ifdef HAVE_BAGL

#include <stdbool.h>  // bool
#include <string.h>   // memset

#include "os.h"
#include "ux.h"
#include "glyphs.h"

#include "display.h"
#include "constants.h"
#include "../globals.h"
#include "io.h"
#include "../sw.h"
#include "../address.h"
#include "action/validate.h"
#include "../types.h"
#include "../transaction/types.h"
#include "../transaction/utils.h"
#include "bip32.h"
#include "../common/format_local.h"
#include "format.h"
#include "../menu.h"

static action_validate_cb g_validate_callback;
static char g_amount[30];
static char g_bip32_path[60];
static char g_address[ECDSA_ADDRESS_LEN + 6];
static char g_fees[30];
static char g_message[MAX_MESSAGE_LEN + 6];

// Validate/Invalidate public key and go back to home
static void ui_action_validate_pubkey(bool choice) {
    validate_pubkey(choice);
    ui_menu_main();
}

// Validate/Invalidate transaction and go back to home
static void ui_action_validate_transaction(bool choice) {
    validate_transaction(choice);
    ui_menu_main();
}

// Validate/Invalidate message and go back to home
static void ui_action_validate_message(bool choice) {
    validate_message(choice);
    ui_menu_main();
}

// Step with icon and text
UX_STEP_NOCB(ux_display_confirm_addr_step, pn, {&C_icon_eye, "Confirm Address"});
// Step with title/text for BIP32 path
UX_STEP_NOCB(ux_display_path_step,
             bnnn_paging,
             {
                 .title = "Path",
                 .text = g_bip32_path,
             });
// Step with title/text for address
UX_STEP_NOCB(ux_display_address_step,
             bnnn_paging,
             {
                 .title = "Address",
                 .text = g_address,
             });
// Step with approve button
UX_STEP_CB(ux_display_approve_step,
           pb,
           (*g_validate_callback)(true),
           {
               &C_icon_validate_14,
               "Approve",
           });
// Step with reject button
UX_STEP_CB(ux_display_reject_step,
           pb,
           (*g_validate_callback)(false),
           {
               &C_icon_crossmark,
               "Reject",
           });

// FLOW to display address and BIP32 path:
// #1 screen: eye icon + "Confirm Address"
// #2 screen: display BIP32 Path
// #3 screen: display address
// #4 screen: approve button
// #5 screen: reject button
UX_FLOW(ux_display_pubkey_flow,
        &ux_display_confirm_addr_step,
        &ux_display_path_step,
        &ux_display_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_display_address() {
    if (G_context.req_type != CONFIRM_ADDRESS || G_context.state != STATE_NONE) {
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

    memset(g_address, 0, sizeof(g_address));
    uint8_t address[ECDSA_ADDRESS_LEN] = {0};
    if (!address_from_pubkey(G_context.pk_info.raw_public_key, SCHNORR, address, sizeof(address))) {
        return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }
    snprintf(g_address, sizeof(g_address), "%.*s", sizeof(address), address);

    g_validate_callback = &ui_action_validate_pubkey;

    ux_flow_init(0, ux_display_pubkey_flow, NULL);
    return 0;
}

// Step with icon and text
UX_STEP_NOCB(ux_display_review_step,
             pnn,
             {
                 &C_icon_eye,
                 "Review",
                 "Transaction",
             });
// Step with title/text for amount
UX_STEP_NOCB(ux_display_amount_step,
             bnnn_paging,
             {
                 .title = "Amount",
                 .text = g_amount,
             });

UX_STEP_NOCB(ux_display_fees_step,
             bnnn_paging,
             {
                 .title = "Fees",
                 .text = g_fees,
             });

// FLOW to display transaction information:
// #1 screen : eye icon + "Review Transaction"
// #2 screen : display address
// #3 screen : display amount
// #4 screen : display fees
// #5 screen : approve button
// #6 screen : reject button
UX_FLOW(ux_display_transaction_flow,
        &ux_display_review_step,
        &ux_display_address_step,
        &ux_display_amount_step,
        &ux_display_fees_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_display_transaction() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    memset(g_amount, 0, sizeof(g_amount));
    char amount[30] = {0};
    if (!format_fpu64_trimmed(amount,
                              sizeof(amount),
                              G_context.tx_info.transaction.tx_outputs[0].value,
                              EXPONENT_SMALLEST_UNIT)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    snprintf(g_amount, sizeof(g_amount), "KAS %.*s", sizeof(amount), amount);
    PRINTF("Amount: %s\n", g_amount);

    memset(g_fees, 0, sizeof(g_fees));
    char fees[30] = {0};
    if (!format_fpu64_trimmed(fees,
                              sizeof(fees),
                              calc_fees(G_context.tx_info.transaction.tx_inputs,
                                        G_context.tx_info.transaction.tx_input_len,
                                        G_context.tx_info.transaction.tx_outputs,
                                        G_context.tx_info.transaction.tx_output_len),
                              EXPONENT_SMALLEST_UNIT)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    snprintf(g_fees, sizeof(g_fees), "KAS %.*s", sizeof(fees), fees);

    memset(g_address, 0, sizeof(g_address));

    uint8_t address[ECDSA_ADDRESS_LEN] = {0};

    if (!script_public_key_to_address(
            address,
            sizeof(address),
            G_context.tx_info.transaction.tx_outputs[0].script_public_key,
            sizeof(G_context.tx_info.transaction.tx_outputs[0].script_public_key))) {
        return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }
    snprintf(g_address, sizeof(g_address), "%.*s", ECDSA_ADDRESS_LEN, address);

    g_validate_callback = &ui_action_validate_transaction;

    ux_flow_init(0, ux_display_transaction_flow, NULL);

    return 0;
}

// Step with icon and text
UX_STEP_NOCB(ux_display_confirm_message_step, pn, {&C_icon_eye, "Review Message"});

// Step with title/text for message
UX_STEP_NOCB(ux_display_message_step,
             bnnn_paging,
             {
                 .title = "Message",
                 .text = g_message,
             });

// FLOW to display address and BIP32 path:
// #1 screen: eye icon + "Confirm Address"
// #2 screen: display BIP32 Path
// #3 screen: display address
// #4 screen: approve button
// #5 screen: reject button
UX_FLOW(ux_display_message_flow,
        &ux_display_confirm_message_step,
        &ux_display_path_step,
        &ux_display_message_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

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
                           (int) sizeof(g_message),
                           (char *) G_context.msg_info.message,
                           (int) G_context.msg_info.message_len);

    g_validate_callback = &ui_action_validate_message;

    ux_flow_init(0, ux_display_message_flow, NULL);
    return 0;
}

#endif
