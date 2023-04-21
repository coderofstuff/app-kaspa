#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <cmocka.h>

#include "transaction/serialize.h"
#include "transaction/deserialize.h"
#include "transaction/types.h"

static void test_tx_serialization(void **state) {
    (void) state;

    transaction_t tx;
    tx.tx_output_len = 2;
    tx.tx_input_len = 3;
    
    // clang-format off
    uint8_t raw_tx[] = {
        // header
        0x00, 0x01, 0x02, 0x03
    };

    buffer_t buf = {.ptr = raw_tx, .size = sizeof(raw_tx), .offset = 0};

    parser_status_e status = transaction_deserialize(&buf, &tx);

    assert_int_equal(status, PARSING_OK);
    assert_int_equal(tx.version, 1);

    // uint8_t output[350];
    // int length = transaction_serialize(&tx, output, sizeof(output));
    // assert_int_equal(length, sizeof(raw_tx));
    // assert_memory_equal(raw_tx, output, sizeof(raw_tx));
}

static void test_tx_input_serialization(void **state) {
        (void) state;

    transaction_input_t txin;

    // clang-format off
    uint8_t raw_tx[] = {
        // Input
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x82, 0xb8,
        0x20, 0xe9, 0xed, 0xf6, 0x7a, 0x32, 0x58, 0x68,
        0xec, 0xc7, 0xcd, 0x85, 0x19, 0xe6, 0xca, 0x52,
        0x65, 0xe6, 0x5b, 0x7d, 0x10, 0xf5, 0x60, 0x66,
        0x46, 0x1c, 0xea, 0xbf, 0x0c, 0x2b, 0xc1, 0xc5,
        0xad, 0xac, 0x00, 0x00, 0x00, 0x00
    };

    buffer_t buf = {.ptr = raw_tx, .size = sizeof(raw_tx), .offset = 0};

    parser_status_e status = transaction_input_deserialize(&buf, &txin);

    assert_int_equal(status, PARSING_OK);

}

static void test_tx_output_serialization_32_bytes(void **state) {
        (void) state;

    transaction_output_t txout;

    // clang-format off
    uint8_t raw_tx[] = {
        // Output
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x86, 0xa0,
        0x20,
        0xe1, 0x19, 0xd5, 0x35, 0x14, 0xc1, 0xb0, 0xe2,
        0xef, 0xce, 0x7a, 0x89, 0xe3, 0xd1, 0xd5, 0xd6,
        0xcd, 0x73, 0x58, 0x2e, 0xa2, 0x06, 0x87, 0x64,
        0x1c, 0x8f, 0xdc, 0xcb, 0x60, 0x60, 0xa9, 0xad,
        0xac
    };

    buffer_t buf = {.ptr = raw_tx, .size = sizeof(raw_tx), .offset = 0};

    parser_status_e status = transaction_output_deserialize(&buf, &txout);

    assert_int_equal(status, PARSING_OK);

}

static void test_tx_output_serialization_33_bytes(void **state) {
        (void) state;

    transaction_output_t txout;

    // clang-format off
    uint8_t raw_tx[] = {
        // Output
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x86, 0xa0,
        0x21,
        0xe1, 0x19, 0xd5, 0x35, 0x14, 0xc1, 0xb0, 0xe2,
        0xef, 0xce, 0x7a, 0x89, 0xe3, 0xd1, 0xd5, 0xd6,
        0xcd, 0x73, 0x58, 0x2e, 0xa2, 0x06, 0x87, 0x64,
        0x1c, 0x8f, 0xdc, 0xcb, 0x60, 0x60, 0xa9, 0xad,
        0x00, 0xac
    };

    buffer_t buf = {.ptr = raw_tx, .size = sizeof(raw_tx), .offset = 0};

    parser_status_e status = transaction_output_deserialize(&buf, &txout);

    assert_int_equal(status, PARSING_OK);

}

int main() {
    const struct CMUnitTest tests[] = {cmocka_unit_test(test_tx_serialization),
                                       cmocka_unit_test(test_tx_input_serialization),
                                       cmocka_unit_test(test_tx_output_serialization_32_bytes),
                                       cmocka_unit_test(test_tx_output_serialization_33_bytes)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}
