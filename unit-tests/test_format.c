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
#include <string.h>

#include <cmocka.h>

#include "common/format.h"

static void test_format_i64(void **state) {
    (void) state;

    char temp[22] = {0};

    int64_t value = 0;
    assert_true(format_i64(temp, sizeof(temp), value));
    assert_string_equal(temp, "0");

    value = (int64_t) 9223372036854775807ull;  // MAX_INT64
    memset(temp, 0, sizeof(temp));
    assert_true(format_i64(temp, sizeof(temp), value));
    assert_string_equal(temp, "9223372036854775807");

    // buffer too small
    assert_false(format_i64(temp, sizeof(temp) - 5, value));

    value = (int64_t) -9223372036854775808ull;  // MIN_INT64
    memset(temp, 0, sizeof(temp));
    assert_true(format_i64(temp, sizeof(temp), value));
    assert_string_equal(temp, "-9223372036854775808");
}

static void test_format_u64(void **state) {
    (void) state;

    char temp[21] = {0};

    uint64_t value = 0;
    assert_true(format_u64(temp, sizeof(temp), value));
    assert_string_equal(temp, "0");

    value = (uint64_t) 18446744073709551615ull;  // MAX_UNT64
    memset(temp, 0, sizeof(temp));
    assert_true(format_u64(temp, sizeof(temp), value));
    assert_string_equal(temp, "18446744073709551615");

    // buffer too small
    assert_false(format_u64(temp, sizeof(temp) - 5, value));
}

static void test_format_fpu64(void **state) {
    (void) state;

    char temp[22] = {0};

    uint64_t amount = 0ull;  // satoshi
    memset(temp, 0, sizeof(temp));
    assert_true(format_fpu64(temp, sizeof(temp), amount, 8));
    assert_string_equal(temp, "0");  // BTC

    amount = 100000000ull;  // satoshi
    memset(temp, 0, sizeof(temp));
    assert_true(format_fpu64(temp, sizeof(temp), amount, 8));
    assert_string_equal(temp, "1");  // BTC

    amount = 24964823ull;  // satoshi
    memset(temp, 0, sizeof(temp));
    assert_true(format_fpu64(temp, sizeof(temp), amount, 8));
    assert_string_equal(temp, "0.24964823");  // BTC

    amount = 100ull;  // satoshi
    memset(temp, 0, sizeof(temp));
    assert_true(format_fpu64(temp, sizeof(temp), amount, 8));
    assert_string_equal(temp, "0.000001");  // BTC
    // buffer too small
    assert_false(format_fpu64(temp, sizeof(temp) - 16, amount, 8));

    char temp2[50] = {0};

    amount = 1000000000000000000ull;  // wei
    assert_true(format_fpu64(temp2, sizeof(temp2), amount, 18));
    assert_string_equal(temp2, "1");  // ETH

    amount = 100000000000000000ull;  // wei
    assert_true(format_fpu64(temp2, sizeof(temp2), amount, 18));
    assert_string_equal(temp2, "0.1");  // ETH

    // buffer too small
    amount = 1000000000000000001ull;  // wei
    assert_false(format_fpu64(temp2, sizeof(temp2) - 20, amount, 18));
}

static void test_format_hex(void **state) {
    (void) state;

    uint8_t address[] = {0xde, 0xb,  0x29, 0x56, 0x69, 0xa9, 0xfd, 0x93, 0xd5, 0xf2,
                         0x8d, 0x9e, 0xc8, 0x5e, 0x40, 0xf4, 0xcb, 0x69, 0x7b, 0xae};
    char output[2 * sizeof(address) + 1] = {0};

    assert_int_equal(2 * sizeof(address) + 1,
                     format_hex(address, sizeof(address), output, sizeof(output)));
    assert_string_equal(output, "DE0B295669A9FD93D5F28D9EC85E40F4CB697BAE");
    assert_int_equal(-1, format_hex(address, sizeof(address), output, sizeof(address)));
}

int main() {
    const struct CMUnitTest tests[] = {cmocka_unit_test(test_format_i64),
                                       cmocka_unit_test(test_format_u64),
                                       cmocka_unit_test(test_format_fpu64),
                                       cmocka_unit_test(test_format_hex)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}
