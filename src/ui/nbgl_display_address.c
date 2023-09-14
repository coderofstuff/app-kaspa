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
#include "../common/bip32.h"
#include "../common/format.h"
#include "../menu.h"

static char g_address[43];
static char g_bip32_path[60];

static nbgl_layoutTagValue_t pairs[1];
static nbgl_layoutTagValueList_t pairList;

static void confirm_address_rejection(void) {
    // display a status page and go back to main
    validate_pubkey(false);
    nbgl_useCaseStatus("Address verification\ncancelled", false, ui_menu_main);
}

static void confirm_address_approval(void) {
    // display a success status page and go back to main
    validate_pubkey(true);
    nbgl_useCaseStatus("ADDRESS\nVERIFIED", true, ui_menu_main);
}

static void review_choice(bool confirm) {
    if (confirm) {
        confirm_address_approval();
    } else {
        confirm_address_rejection();
    }
}

static void continue_review(void) {
    // Fill pairs
    pairs[0].item = "BIP32 Path";
    pairs[0].value = g_bip32_path;

    pairList.nbMaxLinesForValue = 0;
    pairList.nbPairs = 1;
    pairList.pairs = pairs;
    nbgl_useCaseAddressConfirmationExt(g_address, review_choice, &pairList);
}

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
    uint8_t address[SCHNORR_ADDRESS_LEN] = {0};
    if (!address_from_pubkey(G_context.pk_info.raw_public_key, SCHNORR, address, sizeof(address))) {
        return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }
    snprintf(g_address, sizeof(g_address), "%.*s", sizeof(address), address);

    nbgl_useCaseReviewStart(&C_stax_app_kaspa_64px,
                            "Verify KAS address",
                            NULL,
                            "Cancel",
                            continue_review,
                            confirm_address_rejection);
    return 0;
}

#endif
