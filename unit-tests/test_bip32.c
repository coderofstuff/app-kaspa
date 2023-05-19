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

#include "common/bip32.h"

static void test_bip32_format(void **state) {
    (void) state;

    char output[30];
    bool b = false;

    b = bip32_path_format((const uint32_t[5]){0x8000002C, 0x80000000, 0x80000000, 0, 0},
                          5,
                          output,
                          sizeof(output));
    assert_true(b);
    assert_string_equal(output, "44'/0'/0'/0/0");

    b = bip32_path_format((const uint32_t[5]){0x8000002C, 0x80000001, 0x80000000, 0, 0},
                          5,
                          output,
                          sizeof(output));
    assert_true(b);
    assert_string_equal(output, "44'/1'/0'/0/0");

    b = bip32_path_format((const uint32_t[5]){0x8000002C, 0x8001b207, 0x80000000, 0, 0},
                          5,
                          output,
                          sizeof(output));
    assert_true(b);
    // Hex APDU: e005010015058000002c8001b207800000000000000000000000
    assert_string_equal(output, "44'/111111'/0'/0/0");
}

static void test_bad_bip32_format(void **state) {
    (void) state;

    char output[30];
    bool b = true;

    // More than MAX_BIP32_PATH (=10)
    b = bip32_path_format(
        (const uint32_t[11]){0x8000002C, 0x80000000, 0x80000000, 0, 0, 0, 0, 0, 0, 0, 0},
        11,
        output,
        sizeof(output));
    assert_false(b);

    // No BIP32 path (=0)
    b = bip32_path_format(NULL, 0, output, sizeof(output));
    assert_false(b);
}

static void test_bip32_read(void **state) {
    (void) state;

    // clang-format off
    uint8_t input[20] = {
        0x80, 0x00, 0x00, 0x2C,
        0x80, 0x00, 0x00, 0x01,
        0x80, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };
    uint32_t expected[5] = {0x8000002C, 0x80000001, 0x80000000, 0, 0};
    uint32_t output[5] = {0};
    bool b = false;

    b = bip32_path_read(input, sizeof(input), output, 5);
    assert_true(b);
    assert_memory_equal(output, expected, 5);
}

static void test_bad_bip32_read(void **state) {
    (void) state;

    // clang-format off
    uint8_t input[20] = {
        0x80, 0x00, 0x00, 0x2C,
        0x80, 0x00, 0x00, 0x01,
        0x80, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };
    uint32_t output[10] = {0};

    // buffer too small (5 BIP32 paths instead of 10)
    assert_false(bip32_path_read(input, sizeof(input), output, 10));

    // No BIP32 path
    assert_false(bip32_path_read(input, sizeof(input), output, 0));

    // More than MAX_BIP32_PATH (=10)
    assert_false(bip32_path_read(input, sizeof(input), output, 20));
}

int main() {
    const struct CMUnitTest tests[] = {cmocka_unit_test(test_bip32_format),
                                       cmocka_unit_test(test_bad_bip32_format),
                                       cmocka_unit_test(test_bip32_read),
                                       cmocka_unit_test(test_bad_bip32_read)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}
