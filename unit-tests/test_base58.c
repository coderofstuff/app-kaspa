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

#include "common/base58.h"

static void test_base58(void **state) {
    (void) state;

    const char in[] = "USm3fpXnKG5EUBx2ndxBDMPVciP5hGey2Jh4NDv6gmeo1LkMeiKrLJUUBk6Z";
    const char expected_out[] = "The quick brown fox jumps over the lazy dog.";
    uint8_t out[100] = {0};
    int out_len = base58_decode(in, sizeof(in) - 1, out, sizeof(out));
    assert_int_equal(out_len, strlen(expected_out));
    assert_string_equal((char *) out, expected_out);

    const char in2[] = "The quick brown fox jumps over the lazy dog.";
    const char expected_out2[] = "USm3fpXnKG5EUBx2ndxBDMPVciP5hGey2Jh4NDv6gmeo1LkMeiKrLJUUBk6Z";
    char out2[100] = {0};
    int out_len2 = base58_encode((uint8_t *) in2, sizeof(in2) - 1, out2, sizeof(out2));
    assert_int_equal(out_len2, strlen(expected_out2));
    assert_string_equal((char *) out2, expected_out2);
}

int main() {
    const struct CMUnitTest tests[] = {cmocka_unit_test(test_base58)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}
