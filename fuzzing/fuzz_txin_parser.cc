#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

extern "C" {
#include "common/buffer.h"
#include "common/format.h"
#include "transaction/deserialize.h"
#include "transaction/types.h"
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    buffer_t buf = {.ptr = data, .size = size, .offset = 0};
    transaction_input_t txin;
    parser_status_e status;
    char address_type[2] = {0};
    char address_index[5] = {0};
    char sequence[9] = {0};
    char value[9] = {0};
    char tx_id[33] = {0};
    char index[2] = {0};

    memset(&txin, 0, sizeof(txin));

    status = transaction_input_deserialize(&buf, &txin);

    if (status == PARSING_OK) {
        format_u64(address_type, sizeof(address_type), txin.address_type);
        format_u64(address_index, sizeof(address_index), txin.address_index);
        format_u64(sequence, sizeof(sequence), txin.sequence);
        format_u64(value, sizeof(value), txin.value);
        format_hex(txin.tx_id, sizeof(txin.tx_id), tx_id, sizeof(tx_id));
        format_u64(index, sizeof(index), txin.index);
    }

    return 0;
}
