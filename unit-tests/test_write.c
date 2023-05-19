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

#include "common/write.h"

static void test_write(void **state) {
    (void) state;

    uint8_t tmp2[2] = {0};

    uint8_t expected2[2] = {0x01, 0x07};
    write_u16_be(tmp2, 0, (uint16_t) 263U);
    assert_memory_equal(tmp2, expected2, sizeof(expected2));

    memset(tmp2, 0, sizeof(tmp2));
    expected2[0] = 0x07;
    expected2[1] = 0x01;
    write_u16_le(tmp2, 0, (uint16_t) 263U);
    assert_memory_equal(tmp2, expected2, sizeof(expected2));

    uint8_t tmp4[4] = {0};

    uint8_t expected4[4] = {0x01, 0x3B, 0xAC, 0xC7};
    write_u32_be(tmp4, 0, (uint32_t) 20688071UL);
    assert_memory_equal(tmp4, expected4, sizeof(expected4));

    memset(tmp4, 0, sizeof(tmp4));
    expected4[0] = 0xC7;
    expected4[1] = 0xAC;
    expected4[2] = 0x3B;
    expected4[3] = 0x01;
    write_u32_le(tmp4, 0, (uint32_t) 20688071UL);
    assert_memory_equal(tmp4, expected4, sizeof(expected4));

    uint8_t tmp8[8] = {0};

    uint8_t expected8[8] = {0xEB, 0x68, 0x44, 0xC0, 0x2C, 0x61, 0xB0, 0x99};
    write_u64_be(tmp8, 0, (uint64_t) 16962883588659982489ULL);
    assert_memory_equal(tmp8, expected8, sizeof(expected8));

    memset(tmp8, 0, sizeof(tmp8));
    expected8[0] = 0x99;
    expected8[1] = 0xB0;
    expected8[2] = 0x61;
    expected8[3] = 0x2C;
    expected8[4] = 0xC0;
    expected8[5] = 0x44;
    expected8[6] = 0x68;
    expected8[7] = 0xEB;
    write_u64_le(tmp8, 0, (uint64_t) 16962883588659982489ULL);
    assert_memory_equal(tmp8, expected8, sizeof(expected8));
}

int main() {
    const struct CMUnitTest tests[] = {cmocka_unit_test(test_write)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}
