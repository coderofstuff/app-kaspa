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
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

extern "C" {
#include "buffer.h"
#include "common/format.h"
#include "transaction/deserialize.h"
#include "transaction/types.h"
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    buffer_t buf = {.ptr = data, .size = size, .offset = 0};
    transaction_t tx;
    parser_status_e status;
    uint32_t bip32_path[5];
    char version[3] = {0};
    char tx_input_len[3] = {0};
    char tx_output_len[3] = {0};

    memset(&tx, 0, sizeof(tx));

    status = transaction_deserialize(&buf, &tx, bip32_path);

    if (status == PARSING_OK) {
        format_u64(version, sizeof(version), tx.version);
        // printf("version: %s\n", version);
        format_u64(tx_output_len, sizeof(tx_output_len), tx.tx_output_len);
        // printf("tx_output_len: %s\n", tx_output_len);
        format_u64(tx_input_len, sizeof(tx_input_len), tx.tx_input_len);
        // printf("tx_input_len: %s\n", tx_input_len);
    }

    return 0;
}