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

#include "buffer.h"
#include "format.h"
#include "transaction/deserialize.h"
#include "transaction/types.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
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
