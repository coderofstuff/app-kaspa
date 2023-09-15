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
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdbool.h>

#include <cmocka.h>

#include "./import/blake2-impl.h"
#include "./import/blake2b.h"
#include "personal_message.h"

/* Start hacks */
void os_longjmp(unsigned int exception) {}

struct cx_xblake_s {
    cx_blake2b_t blake2b;
    uint64_t     m[16];
    uint64_t     v[16];
    uint8_t      buffer[BLAKE2B_OUTBYTES];
    uint8_t      block1[BLAKE2B_BLOCKBYTES];
} ;
typedef struct cx_xblake_s cx_xblake_t;

union cx_u {
  cx_xblake_t blake;
};
union cx_u G_cx;

uint8_t* input_public_key;

/* End hacks */

static void test_hash_personal_message_vector0(void **state) {
    char message[] = "Hello Kaspa!"; // 12 chars

    uint8_t out_hash[32] = {0};

    bool result = hash_personal_message((uint8_t*) message, 12, out_hash);

    assert_true(result);

    uint8_t res[32] = {0x2E, 0x55, 0xDE, 0xDA, 0x4A, 0x52, 0x24, 0x20,
                       0x8D, 0xBD, 0x4D, 0x93, 0xCF, 0xE5, 0xAA, 0x22,
                       0xD9, 0x45, 0xEA, 0xA6, 0x31, 0x72, 0xE3, 0x29,
                       0xC4, 0x9A, 0xFE, 0xD6, 0x2F, 0x0E, 0x15, 0x10};

    assert_memory_equal(out_hash, res, 32);
}

// static void test_sign_message_vector1(void **state) {
//     uint8_t private_key_data[32] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03};

//     char message[] = "Hello Kaspa!"; // 12 chars

//     uint8_t out_hash[64] = {0};

//     bool result = hash_personal_message(private_key_data, (uint8_t*) message, 12, out_hash);

//     assert_true(result);

//     uint8_t res[64] = {0x40, 0xB9, 0xBB, 0x2B, 0xE0, 0xAE, 0x02, 0x60,
//                        0x72, 0x79, 0xED, 0xA6, 0x40, 0x15, 0xA8, 0xD8,
//                        0x6E, 0x37, 0x63, 0x27, 0x91, 0x70, 0x34, 0x0B,
//                        0x82, 0x43, 0xF7, 0xCE, 0x53, 0x44, 0xD7, 0x7A,
//                        0xFF, 0x11, 0x91, 0x59, 0x8B, 0xAF, 0x2F, 0xD2,
//                        0x61, 0x49, 0xCA, 0xC3, 0xB4, 0xB1, 0x2C, 0x2C,
//                        0x43, 0x32, 0x61, 0xC0, 0x08, 0x34, 0xDB, 0x60,
//                        0x98, 0xCB, 0x17, 0x2A, 0xA4, 0x8E, 0xF5, 0x22};

//     assert_memory_equal(out_hash, res, 64);
// }

//612d56e633ee5da1caa4563c6ace0c98d3549ad4e3d2b1f1ea6810e6c34047bd


int main() {
    const struct CMUnitTest tests[] = {cmocka_unit_test(test_hash_personal_message_vector0)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}
