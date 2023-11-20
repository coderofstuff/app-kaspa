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

#include "types.h"
#include "parser.h"

static void test_apdu_parser(void **state) {
    (void) state;
    uint8_t apdu_bad_min_len[] = {0xE0, 0x03, 0x00, 0x00};   // less than 5 bytes
    uint8_t apdu_bad_lc[] = {0xE0, 0x03, 0x00, 0x00, 0x01};  // Lc = 1 but no data
    uint8_t apdu[] = {0xE0, 0x03, 0x01, 0x02, 0x05, 0x00, 0x01, 0x02, 0x03, 0x04};

    command_t cmd;

    memset(&cmd, 0, sizeof(cmd));
    assert_true(apdu_parser(&cmd, apdu_bad_min_len, sizeof(apdu_bad_min_len)));
    assert_int_equal(cmd.lc, 0);

    memset(&cmd, 0, sizeof(cmd));
    assert_false(apdu_parser(&cmd, apdu_bad_lc, sizeof(apdu_bad_lc)));

    memset(&cmd, 0, sizeof(cmd));
    assert_true(apdu_parser(&cmd, apdu, sizeof(apdu)));
    assert_int_equal(cmd.cla, 0xE0);
    assert_int_equal(cmd.ins, 0x03);
    assert_int_equal(cmd.p1, 0x01);
    assert_int_equal(cmd.p2, 0x02);
    assert_int_equal(cmd.lc, 5);
    assert_memory_equal(cmd.data, ((uint8_t[]){0x00, 0x01, 0x02, 0x03, 0x04}), cmd.lc);
}

int main() {
    const struct CMUnitTest tests[] = {cmocka_unit_test(test_apdu_parser)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}
