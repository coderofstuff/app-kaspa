#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdbool.h>

#include <cmocka.h>

#include "./import/blake2-impl.h"
#include "./import/blake2b.h"
#include "sighash.h"
#include "./transaction/types.h"
#include "types.h"

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

global_ctx_t G_context;

uint8_t* input_public_key;

/* End hacks */

static void test_sighash(void **state) {
    uint8_t input_public_key_data[32] = {0xe9, 0xed, 0xf6, 0x7a, 0x32, 0x58, 0x68, 0xec,
                                    0xc7, 0xcd, 0x85, 0x19, 0xe6, 0xca, 0x52, 0x65,
                                    0xe6, 0x5b, 0x7d, 0x10, 0xf5, 0x60, 0x66, 0x46,
                                    0x1c, 0xea, 0xbf, 0x0c, 0x2b, 0xc1, 0xc5, 0xad};

    uint8_t input_prev_tx_id[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                                    0x99, 0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
                                    0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                                    0x99, 0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

    uint8_t output_script_public_key[34] = {0x20,
                                            0xc6, 0x2c, 0xf3, 0x0e, 0x4e, 0x57, 0xc5, 0x92,
                                            0x20, 0x86, 0x46, 0x02, 0x35, 0xd5, 0x15, 0x7a,
                                            0x62, 0xf2, 0x66, 0x6a, 0xa2, 0x27, 0x07, 0xcb,
                                            0x9b, 0x58, 0x8f, 0x62, 0x11, 0xfc, 0x0e, 0xd6,
                                            0xac};
                                

    transaction_input_t txin;
    transaction_output_t txout;
    transaction_t tx;

    memcpy(txin.tx_id, input_prev_tx_id, sizeof(input_prev_tx_id));
    txin.index = 1;
    txin.value = 2;

    memcpy(txout.script_public_key, output_script_public_key, sizeof(output_script_public_key));
    txout.value = txin.value; // Assume no fee

    tx.version = 1;
    tx.tx_inputs[0] = txin;
    tx.tx_input_len = 1;
    tx.tx_outputs[0] = txout;
    tx.tx_output_len = 1;

    uint8_t out_hash[32] = {0};
    calc_sighash(&tx, &txin, input_public_key_data, out_hash);

    uint8_t res[32] = {0x7c, 0xcd, 0xa6, 0xc6, 0x4a, 0x18, 0x1e, 0x62,
                       0x63, 0xf0, 0xee, 0xe2, 0xed, 0xc8, 0x59, 0xdb,
                       0xcd, 0x9d, 0xe7, 0x17, 0xc0, 0x65, 0xea, 0x8e,
                       0x7d, 0xce, 0x10, 0x81, 0xbe, 0xc5, 0xba, 0xa5};

    assert_memory_equal(out_hash, res, 32);
}

//612d56e633ee5da1caa4563c6ace0c98d3549ad4e3d2b1f1ea6810e6c34047bd
static void test_sighash_zeros(void **state) {
    uint8_t input_public_key_data[32] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    uint8_t input_prev_tx_id[32] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    uint8_t output_script_public_key[34] = {0x20,
                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                            0xac};
                                

    transaction_input_t txin;
    transaction_output_t txout;
    transaction_t tx;

    memcpy(txin.tx_id, input_prev_tx_id, sizeof(input_prev_tx_id));
    txin.index = 0;
    txin.value = 0;

    memcpy(txout.script_public_key, output_script_public_key, sizeof(output_script_public_key));
    txout.value = txin.value; // Assume no fee

    tx.version = 0;
    tx.tx_inputs[0] = txin;
    tx.tx_input_len = 1;
    tx.tx_outputs[0] = txout;
    tx.tx_output_len = 1;

    uint8_t out_hash[32] = {0};
    calc_sighash(&tx, &txin, input_public_key_data, out_hash);

    uint8_t res[32] = {0x61, 0x2d, 0x56, 0xe6, 0x33, 0xee, 0x5d, 0xa1,
                       0xca, 0xa4, 0x56, 0x3c, 0x6a, 0xce, 0x0c, 0x98,
                       0xd3, 0x54, 0x9a, 0xd4, 0xe3, 0xd2, 0xb1, 0xf1,
                       0xea, 0x68, 0x10, 0xe6, 0xc3, 0x40, 0x47, 0xbd};

    assert_memory_equal(out_hash, res, 32);
}

int main() {
    const struct CMUnitTest tests[] = {cmocka_unit_test(test_sighash),
                                       cmocka_unit_test(test_sighash_zeros)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}
