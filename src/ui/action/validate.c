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

        int error = crypto_sign_message();
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
