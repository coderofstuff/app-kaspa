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

#include "transaction/serialize.h"
#include "transaction/deserialize.h"
#include "transaction/types.h"

static void test_tx_serialization(void **state) {
    (void) state;

    transaction_t tx;
    tx.tx_output_len = 2;
    tx.tx_input_len = 3;

    uint32_t path[5] = {0};
    
    // clang-format off
    uint8_t raw_tx[] = {
        // header
        0x00, 0x01, 0x02, 0x03,
        0x01,
        0x04, 0x05, 0x06, 0xFF
    };

    buffer_t buf = {.ptr = raw_tx, .size = sizeof(raw_tx), .offset = 0};

    parser_status_e status = transaction_deserialize(&buf, &tx, path);

    assert_int_equal(status, PARSING_OK);
    assert_int_equal(tx.version, 1);

    uint8_t output[350];
    int length = transaction_serialize(&tx, path, output, sizeof(output));
    assert_int_equal(length, sizeof(raw_tx));
    assert_memory_equal(raw_tx, output, sizeof(raw_tx));
}

static int run_test_tx_serialize(uint8_t* raw_tx, size_t raw_tx_len) {
    transaction_t tx;

    buffer_t buf = {.ptr = raw_tx, .size = raw_tx_len, .offset = 0};

    uint32_t path[5] = {0};

    return transaction_deserialize(&buf, &tx, path);
}

static void test_tx_deserialization_fail(void **state) {
    (void) state;

    transaction_t tx;
    
    // clang-format off
    uint8_t invalid_version[] = {
        // incomplete version
        0x00
    };

    uint8_t missing_outlen[] = {
        0x00, 0x01
    };

    uint8_t invalid_outlen[] = {
        0x00, 0x01, 0x03
    };

    uint8_t missing_inlen[] = {
        0x00, 0x01, 0x02
    };

    uint8_t invalid_inlen[] = {
        0x00, 0x01, 0x02, 0x00
    };

    uint8_t invalid_change_type[] = {
        0x00, 0x01, 0x02, 0x03, 0x02
    };

    assert_int_equal(run_test_tx_serialize(invalid_version, sizeof(invalid_version)), VERSION_PARSING_ERROR);
    assert_int_equal(run_test_tx_serialize(missing_outlen, sizeof(missing_outlen)), OUTPUTS_LENGTH_PARSING_ERROR);
    assert_int_equal(run_test_tx_serialize(invalid_outlen, sizeof(invalid_outlen)), OUTPUTS_LENGTH_PARSING_ERROR);
    assert_int_equal(run_test_tx_serialize(missing_inlen, sizeof(missing_inlen)), INPUTS_LENGTH_PARSING_ERROR);
    assert_int_equal(run_test_tx_serialize(invalid_inlen, sizeof(invalid_inlen)), INPUTS_LENGTH_PARSING_ERROR);
    assert_int_equal(run_test_tx_serialize(invalid_change_type, sizeof(invalid_change_type)), HEADER_PARSING_ERROR);
}

static void test_tx_input_serialization(void **state) {
        (void) state;

    transaction_input_t txin;

    // clang-format off
    uint8_t raw_tx[] = {
        // Input
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x82, 0xb8,
        0xe9, 0xed, 0xf6, 0x7a, 0x32, 0x58, 0x68, 0xec,
        0xc7, 0xcd, 0x85, 0x19, 0xe6, 0xca, 0x52, 0x65,
        0xe6, 0x5b, 0x7d, 0x10, 0xf5, 0x60, 0x66, 0x46,
        0x1c, 0xea, 0xbf, 0x0c, 0x2b, 0xc1, 0xc5, 0xad,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    buffer_t buf = {.ptr = raw_tx, .size = sizeof(raw_tx), .offset = 0};

    parser_status_e status = transaction_input_deserialize(&buf, &txin);

    assert_int_equal(status, PARSING_OK);

    uint8_t output[350];
    int length = transaction_input_serialize(&txin, output, sizeof(output));
    assert_int_equal(length, sizeof(raw_tx));
    assert_memory_equal(raw_tx, output, sizeof(raw_tx));
}

static int run_test_tx_input_serialize(uint8_t* raw_tx, size_t raw_tx_len) {
    transaction_input_t txin;

    buffer_t buf = {.ptr = raw_tx, .size = raw_tx_len, .offset = 0};

    return transaction_input_deserialize(&buf, &txin);
}

static void test_tx_input_deserialization_fail(void **state) {
    // clang-format off
    uint8_t invalid_value[] = {
        // Value is only 7 bytes
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x82
    };

    uint8_t invalid_tx_id[] = {
        // Txid is only 30
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x82, 0xb8,
        0xe9, 0xed, 0xf6, 0x7a, 0x32, 0x58, 0x68, 0xec,
        0xc7, 0xcd, 0x85, 0x19, 0xe6, 0xca, 0x52, 0x65,
        0xe6, 0x5b, 0x7d, 0x10, 0xf5, 0x60, 0x66, 0x46,
        0x1c, 0xea, 0xbf, 0x0c, 0x2b, 0xc1
    };

    uint8_t invalid_address_type[] = {
        // Missing address_type
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x82, 0xb8,
        0xe9, 0xed, 0xf6, 0x7a, 0x32, 0x58, 0x68, 0xec,
        0xc7, 0xcd, 0x85, 0x19, 0xe6, 0xca, 0x52, 0x65,
        0xe6, 0x5b, 0x7d, 0x10, 0xf5, 0x60, 0x66, 0x46,
        0x1c, 0xea, 0xbf, 0x0c, 0x2b, 0xc1, 0xc5, 0xad
    };

    uint8_t invalid_addres_index[] = {
        // Missing address_index
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x82, 0xb8,
        0xe9, 0xed, 0xf6, 0x7a, 0x32, 0x58, 0x68, 0xec,
        0xc7, 0xcd, 0x85, 0x19, 0xe6, 0xca, 0x52, 0x65,
        0xe6, 0x5b, 0x7d, 0x10, 0xf5, 0x60, 0x66, 0x46,
        0x1c, 0xea, 0xbf, 0x0c, 0x2b, 0xc1, 0xc5, 0xad,
        0x00, 0x00, 0x00, 0x00
    };

    uint8_t invalid_index[] = {
        // Missing address_index
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x82, 0xb8,
        0xe9, 0xed, 0xf6, 0x7a, 0x32, 0x58, 0x68, 0xec,
        0xc7, 0xcd, 0x85, 0x19, 0xe6, 0xca, 0x52, 0x65,
        0xe6, 0x5b, 0x7d, 0x10, 0xf5, 0x60, 0x66, 0x46,
        0x1c, 0xea, 0xbf, 0x0c, 0x2b, 0xc1, 0xc5, 0xad,
        0x00, 0x00, 0x00, 0x00, 0x00
    };

    assert_int_equal(run_test_tx_input_serialize(invalid_value, sizeof(invalid_value)), INPUT_VALUE_PARSING_ERROR);
    assert_int_equal(run_test_tx_input_serialize(invalid_tx_id, sizeof(invalid_tx_id)), INPUT_TX_ID_PARSING_ERROR);
    assert_int_equal(run_test_tx_input_serialize(invalid_address_type, sizeof(invalid_address_type)), INPUT_ADDRESS_TYPE_PARSING_ERROR);
    assert_int_equal(run_test_tx_input_serialize(invalid_addres_index, sizeof(invalid_addres_index)), INPUT_ADDRESS_INDEX_PARSING_ERROR);
    assert_int_equal(run_test_tx_input_serialize(invalid_index, sizeof(invalid_index)), INPUT_INDEX_PARSING_ERROR);
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
        OP_CHECKSIG
    };

    buffer_t buf = {.ptr = raw_tx, .size = sizeof(raw_tx), .offset = 0};

    parser_status_e status = transaction_output_deserialize(&buf, &txout);

    assert_int_equal(status, PARSING_OK);

    uint8_t output[350];
    int length = transaction_output_serialize(&txout, output, sizeof(output));
    assert_int_equal(length, sizeof(raw_tx));
    assert_memory_equal(raw_tx, output, sizeof(raw_tx));
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
        0x00, OP_CHECKSIGECDSA
    };

    buffer_t buf = {.ptr = raw_tx, .size = sizeof(raw_tx), .offset = 0};

    parser_status_e status = transaction_output_deserialize(&buf, &txout);

    assert_int_equal(status, PARSING_OK);

    uint8_t output[350];
    int length = transaction_output_serialize(&txout, output, sizeof(output));
    assert_int_equal(length, sizeof(raw_tx));
    assert_memory_equal(raw_tx, output, sizeof(raw_tx));
}

static int run_test_tx_output_serialize(uint8_t* raw_tx, size_t raw_tx_len) {
    transaction_output_t txout;

    buffer_t buf = {.ptr = raw_tx, .size = raw_tx_len, .offset = 0};

    return transaction_output_deserialize(&buf, &txout);
}

static void test_tx_output_deserialization_fail(void **state) {
        (void) state;

    transaction_output_t txout;

    // clang-format off
    uint8_t invalid_value[] = {
        // Value is only 7 bytes
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x86
    };

    uint8_t invalid_script_start[] = {
        // Start is not 0x20 or 0x21
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x86, 0xa0,
        0x30,
        0xe1, 0x19, 0xd5, 0x35, 0x14, 0xc1, 0xb0, 0xe2,
        0xef, 0xce, 0x7a, 0x89, 0xe3, 0xd1, 0xd5, 0xd6,
        0xcd, 0x73, 0x58, 0x2e, 0xa2, 0x06, 0x87, 0x64,
        0x1c, 0x8f, 0xdc, 0xcb, 0x60, 0x60, 0xa9, 0xad,
        0x00, OP_CHECKSIGECDSA
    };

    uint8_t invalid_script_end_ecdsa[] = {
        // End is not 0xad
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x86, 0xa0,
        0x21,
        0xe1, 0x19, 0xd5, 0x35, 0x14, 0xc1, 0xb0, 0xe2,
        0xef, 0xce, 0x7a, 0x89, 0xe3, 0xd1, 0xd5, 0xd6,
        0xcd, 0x73, 0x58, 0x2e, 0xa2, 0x06, 0x87, 0x64,
        0x1c, 0x8f, 0xdc, 0xcb, 0x60, 0x60, 0xa9, 0xad,
        0x00, 0xff
    };

    uint8_t invalid_script_end_schnorr[] = {
        // End is not 0xac
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x86, 0xa0,
        0x20,
        0xe1, 0x19, 0xd5, 0x35, 0x14, 0xc1, 0xb0, 0xe2,
        0xef, 0xce, 0x7a, 0x89, 0xe3, 0xd1, 0xd5, 0xd6,
        0xcd, 0x73, 0x58, 0x2e, 0xa2, 0x06, 0x87, 0x64,
        0x1c, 0x8f, 0xdc, 0xcb, 0x60, 0x60, 0xa9, 0xad,
        0xff
    };

    uint8_t invalid_script_len[] = {
        // Output
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x86, 0xa0,
        0x21,
        0xe1, 0x19, 0xd5, 0x35, 0x14, 0xc1, 0xb0, 0xe2,
        0xef, 0xce, 0x7a, 0x89, 0xe3, 0xd1, 0xd5, 0xd6,
        0xcd, 0x73, 0x58, 0x2e, 0xa2, 0x06, 0x87, 0x64,
    };

    uint8_t raw_tx[] = {
        // Output
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x86, 0xa0,
        0x21,
        0xe1, 0x19, 0xd5, 0x35, 0x14, 0xc1, 0xb0, 0xe2,
        0xef, 0xce, 0x7a, 0x89, 0xe3, 0xd1, 0xd5, 0xd6,
        0xcd, 0x73, 0x58, 0x2e, 0xa2, 0x06, 0x87, 0x64,
        0x1c, 0x8f, 0xdc, 0xcb, 0x60, 0x60, 0xa9, 0xad,
        0x00, OP_CHECKSIGECDSA
    };

    assert_int_equal(run_test_tx_output_serialize(invalid_value, sizeof(invalid_value)), OUTPUT_VALUE_PARSING_ERROR);
    assert_int_equal(run_test_tx_output_serialize(invalid_script_start, sizeof(invalid_script_start)), OUTPUT_SCRIPT_PUBKEY_PARSING_ERROR);
    assert_int_equal(run_test_tx_output_serialize(invalid_script_end_ecdsa, sizeof(invalid_script_end_ecdsa)), OUTPUT_SCRIPT_PUBKEY_PARSING_ERROR);
    assert_int_equal(run_test_tx_output_serialize(invalid_script_len, sizeof(invalid_script_len)), OUTPUT_SCRIPT_PUBKEY_PARSING_ERROR);
}

static void test_serialization_fail(void **state) {
    transaction_t tx;
    transaction_output_t txout;
    transaction_input_t txin;

    uint8_t buffer[1] = {0};
    uint32_t path[5] = {0};

    assert_int_equal(transaction_serialize(&tx, path, buffer, sizeof(buffer)), -1);
    assert_int_equal(transaction_output_serialize(&txout, buffer, sizeof(buffer)), -1);
    assert_int_equal(transaction_input_serialize(&txin, buffer, sizeof(buffer)), -1);
}

int main() {
    const struct CMUnitTest tests[] = {cmocka_unit_test(test_tx_serialization),
                                       cmocka_unit_test(test_tx_deserialization_fail),
                                       cmocka_unit_test(test_tx_input_serialization),
                                       cmocka_unit_test(test_tx_input_deserialization_fail),
                                       cmocka_unit_test(test_tx_output_serialization_32_bytes),
                                       cmocka_unit_test(test_tx_output_serialization_33_bytes),
                                       cmocka_unit_test(test_tx_output_deserialization_fail),
                                       cmocka_unit_test(test_serialization_fail)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}
