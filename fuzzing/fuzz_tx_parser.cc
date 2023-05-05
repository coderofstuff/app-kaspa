#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

extern "C" {
#include "common/buffer.h"
#include "common/format.h"
#include "transaction/deserialize.h"
#include "transaction/utils.h"
#include "transaction/types.h"
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    buffer_t buf = {.ptr = data, .size = size, .offset = 0};
    transaction_t tx;
    parser_status_e status;
    char version[3] = {0};

    memset(&tx, 0, sizeof(tx));

    status = transaction_deserialize(&buf, &tx);

    if (status == PARSING_OK) {
        format_u64(version, sizeof(version), tx.version);
    }

    return 0;
}
