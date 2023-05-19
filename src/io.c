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
#include <stdint.h>
#include <string.h>

#include "os.h"
#include "ux.h"

#ifdef HAVE_NBGL
#include "nbgl_touch.h"
#include "nbgl_page.h"
#endif  // HAVE_NBGL

#include "io.h"
#include "globals.h"
#include "sw.h"
#include "common/buffer.h"
#include "common/write.h"

#ifdef HAVE_BAGL
void io_seproxyhal_display(const bagl_element_t *element) {
    io_seproxyhal_display_default(element);
}
#endif  // HAVE_BAGL

uint8_t io_event(uint8_t channel) {
    (void) channel;

    switch (G_io_seproxyhal_spi_buffer[0]) {
        case SEPROXYHAL_TAG_BUTTON_PUSH_EVENT:
#ifdef HAVE_BAGL
            UX_BUTTON_PUSH_EVENT(G_io_seproxyhal_spi_buffer);
#endif  // HAVE_BAGL
            break;
        case SEPROXYHAL_TAG_STATUS_EVENT:
            if (G_io_apdu_media == IO_APDU_MEDIA_USB_HID &&  //
                !(U4BE(G_io_seproxyhal_spi_buffer, 3) &      //
                  SEPROXYHAL_TAG_STATUS_EVENT_FLAG_USB_POWERED)) {
                THROW(EXCEPTION_IO_RESET);
            }
            __attribute__((fallthrough));
        case SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT:
#ifdef HAVE_BAGL
            UX_DISPLAYED_EVENT({});
#endif  // HAVE_BAGL
#ifdef HAVE_NBGL
            UX_DEFAULT_EVENT();
#endif  // HAVE_NBGL
            break;
#ifdef HAVE_NBGL
        case SEPROXYHAL_TAG_FINGER_EVENT:
            UX_FINGER_EVENT(G_io_seproxyhal_spi_buffer);
            break;
#endif  // HAVE_NBGL
        case SEPROXYHAL_TAG_TICKER_EVENT:
            UX_TICKER_EVENT(G_io_seproxyhal_spi_buffer, {});
            break;
        default:
            UX_DEFAULT_EVENT();
            break;
    }

    if (!io_seproxyhal_spi_is_status_sent()) {
        io_seproxyhal_general_status();
    }

    return 1;
}

uint16_t io_exchange_al(uint8_t channel, uint16_t tx_len) {
    switch (channel & ~(IO_FLAGS)) {
        case CHANNEL_KEYBOARD:
            break;
        case CHANNEL_SPI:
            if (tx_len) {
                io_seproxyhal_spi_send(G_io_apdu_buffer, tx_len);

                if (channel & IO_RESET_AFTER_REPLIED) {
                    halt();
                }

                return 0;
            } else {
                return io_seproxyhal_spi_recv(G_io_apdu_buffer, sizeof(G_io_apdu_buffer), 0);
            }
        default:
            THROW(INVALID_PARAMETER);
    }

    return 0;
}

/**
 * Variable containing the length of the APDU response to send back.
 */
static uint32_t G_output_len = 0;

/**
 * IO state (READY, RECEIVING, WAITING).
 */
static io_state_e G_io_state = READY;

void io_init() {
    // Reset length of APDU response
    G_output_len = 0;
    G_io_state = READY;
}

int io_recv_command() {
    int ret = -1;

    switch (G_io_state) {
        case READY:
            G_io_state = RECEIVED;
            ret = io_exchange(CHANNEL_APDU, G_output_len);
            break;
        case RECEIVED:
            G_io_state = WAITING;
            ret = io_exchange(CHANNEL_APDU | IO_ASYNCH_REPLY, G_output_len);
            G_io_state = RECEIVED;
            break;
        case WAITING:
            G_io_state = READY;
            ret = -1;
            break;
    }

    return ret;
}

int io_send_response(const buffer_t *rdata, uint16_t sw) {
    int ret = -1;

    if (rdata != NULL) {
        if (rdata->size - rdata->offset > IO_APDU_BUFFER_SIZE - 2 ||  //
            !buffer_copy(rdata, G_io_apdu_buffer, sizeof(G_io_apdu_buffer))) {
            return io_send_sw(SW_WRONG_RESPONSE_LENGTH);
        }
        G_output_len = rdata->size - rdata->offset;
        PRINTF("<= SW=%04X | RData=%.*H\n", sw, rdata->size, rdata->ptr);
    } else {
        G_output_len = 0;
        PRINTF("<= SW=%04X | RData=\n", sw);
    }

    write_u16_be(G_io_apdu_buffer, G_output_len, sw);
    G_output_len += 2;

    switch (G_io_state) {
        case READY:
            ret = -1;
            break;
        case RECEIVED:
            G_io_state = READY;
            ret = 0;
            break;
        case WAITING:
            ret = io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, G_output_len);
            G_output_len = 0;
            G_io_state = READY;
            break;
    }

    return ret;
}

int io_send_sw(uint16_t sw) {
    return io_send_response(NULL, sw);
}
