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
#include "common/format_local.h"
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
