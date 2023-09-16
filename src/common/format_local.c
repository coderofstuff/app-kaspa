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

#include <stddef.h>   // size_t
#include <stdint.h>   // int*_t, uint*_t
#include <string.h>   // strncpy, memmove
#include <stdbool.h>  // bool
#include <stdio.h>    // sprintf, snprintf
#include <ctype.h>    // isprint, isspace

#include "./format_local.h"

void format_message_to_sign(char* msg_dest, int msg_dest_len, char* msg_src, int msg_src_len) {
    int c;
    int dest_idx = 0;

    for (int i = 0; i < msg_src_len && dest_idx < msg_dest_len; i++) {
        c = msg_src[i];
        if (isspace(c))  // to replace all white-space characters as spaces
        {
            c = ' ';
        }
        if (isprint(c)) {
            sprintf(msg_dest + dest_idx, "%c", (char) c);
            dest_idx++;
        } else {
            int remaining_buffer_length = msg_dest_len - dest_idx - 1;
            if (remaining_buffer_length >= 4)  // 4 being the fixed length of \x00
            {
                snprintf(msg_dest + dest_idx, remaining_buffer_length, "\\x%02x", c);
                dest_idx += 4;
            } else {
                // fill the rest of the UI buffer spaces, to consider the buffer full
                memset(msg_dest + dest_idx, ' ', remaining_buffer_length);
                dest_idx += remaining_buffer_length;
            }
        }
    }
}
