#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool
#include <stddef.h>   // size_t
#include <string.h>   // memset, explicit_bzero

#include "os.h"
#include "cx.h"

#include "sign_tx.h"
#include "../apdu/dispatcher.h"
#include "../sw.h"
#include "../globals.h"
#include "../crypto.h"
#include "../ui/display.h"
#include "../common/buffer.h"
#include "../transaction/types.h"
#include "../transaction/deserialize.h"
#include "../transaction/tx_validate.h"
#include "../helper/send_response.h"

static int sign_input_and_send() {
    int error = crypto_sign_message();
    if (error != 0) {
        G_context.state = STATE_NONE;
        io_send_sw(error);
    } else {
        helper_send_response_sig();
        G_context.tx_info.signing_input_index++;
    }

    return error;
}

int handler_sign_tx(buffer_t *cdata, uint8_t type, bool more) {
    if (type == 0) {
        explicit_bzero(&G_context, sizeof(G_context));
        G_context.req_type = CONFIRM_TRANSACTION;
        G_context.state = STATE_NONE;

        parser_status_e status = transaction_deserialize(cdata, &G_context.tx_info.transaction);

        PRINTF("Header Parsing status: %d.\n", status);

        if (status != PARSING_OK) {
            return io_send_sw(SW_TX_PARSING_FAIL);
        }

        return io_send_sw(SW_OK);

    } else if (type == 1 || type == 2) {  // parse transaction

        if (G_context.req_type != CONFIRM_TRANSACTION) {
            return io_send_sw(SW_BAD_STATE);
        }

        // Parse as we go
        if (type == P1_OUTPUTS) {
            // Outputs
            if (G_context.tx_info.parsing_output_index >=
                    sizeof(G_context.tx_info.transaction.tx_outputs) ||
                G_context.tx_info.parsing_output_index >=
                    G_context.tx_info.transaction.tx_output_len) {
                // Too many outputs!
                return io_send_sw(SW_TX_PARSING_FAIL);
            }

            parser_status_e err = transaction_output_deserialize(
                cdata,
                &G_context.tx_info.transaction.tx_outputs[G_context.tx_info.parsing_output_index]);

            PRINTF("Output Parsing status: %d.\n", err);

            if (err != PARSING_OK) {
                return io_send_sw(err);
            } else {
                G_context.tx_info.parsing_output_index++;
            }

        } else if (type == P1_INPUTS) {
            // Inputs
            if (G_context.tx_info.parsing_input_index >=
                    sizeof(G_context.tx_info.transaction.tx_inputs) ||
                G_context.tx_info.parsing_input_index >=
                    G_context.tx_info.transaction.tx_input_len) {
                // Too many inputs!
                return io_send_sw(SW_TX_PARSING_FAIL);
            }

            parser_status_e err = transaction_input_deserialize(
                cdata,
                &G_context.tx_info.transaction.tx_inputs[G_context.tx_info.parsing_input_index]);

            PRINTF("Input Parsing status: %d.\n", err);

            if (err < 0) {
                return io_send_sw(SW_TX_PARSING_FAIL);
            } else {
                G_context.tx_info.parsing_input_index++;
            }

        } else {
            return io_send_sw(SW_WRONG_P1P2);
        }

        if (more) {
            // more APDUs with transaction part are expected.
            // Send a SW_OK to signal that we have received the chunk
            return io_send_sw(SW_OK);

        } else {
            // Before asking the user, make sure one last time that the inputs are legitimate:
            if (!tx_validate_parsed_transaction(&G_context.tx_info.transaction)) {
                return io_send_sw(SW_TX_PARSING_FAIL);
            }

            // last APDU for this transaction, let's parse, display and request a sign confirmation
            G_context.state = STATE_PARSED;

            return ui_display_transaction();
        }
    } else if (type == 3) {
        if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_APPROVED) {
            explicit_bzero(&G_context, sizeof(G_context));
            G_context.state = STATE_NONE;
            return io_send_sw(SW_BAD_STATE);
        }

        sign_input_and_send();
    } else {
        explicit_bzero(&G_context, sizeof(G_context));
        G_context.state = STATE_NONE;
        return io_send_sw(SW_WRONG_P1P2);
    }

    return 0;
}
