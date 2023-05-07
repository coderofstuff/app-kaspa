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
    transaction_output_t txout;
    parser_status_e status;
    char value[9] = {0};
    char script_public_key[36] = {0};

    memset(&txout, 0, sizeof(txout));

    status = transaction_output_deserialize(&buf, &txout);

    if (status == PARSING_OK) {
        format_u64(value, sizeof(value), txout.value);
        // printf("value: %s\n", value);
        format_hex(txout.script_public_key, 35, script_public_key, sizeof(script_public_key));
        // printf("script_public_key: %s\n", script_public_key);
    }

    return 0;
}
