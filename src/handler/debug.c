#include <stdint.h>
#include <string.h>

#include "cx.h"
#include "io.h"

#include "../sw.h"
#include "../types.h"

#ifdef HAVE_DEBUG_APDU

static int helper_send_response_sig(uint8_t* signature) {
    uint8_t resp[66] = {0};
    size_t offset = 0;

    resp[offset++] = 64;
    memmove(resp + offset, signature, 64);
    offset += 64;

    return io_send_response(&(const buffer_t){.ptr = resp, .size = offset, .offset = 0}, SW_OK);
}
// Test cases from https://github.com/bitcoin/bips/blob/master/bip-0340/test-vectors.csv
static void debug_test_case_1(uint8_t* signature) {
    cx_ecfp_private_key_t private_key = {0};
    uint32_t info = 0;
    int sig_len = 0;
    int signature_len = 72;

    // clang-format off
    uint8_t private_key_data[32] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03};
    // clang-format off
    uint8_t aux_rand[32] =         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t msg[32] = {0};

    memcpy(signature, aux_rand, sizeof(aux_rand));
    memcpy(private_key.d, private_key_data, sizeof(private_key_data));
    private_key.curve = CX_CURVE_SECP256K1;
    private_key.d_len = sizeof(private_key_data);

    sig_len = cx_ecschnorr_sign(&private_key,
        CX_ECSCHNORR_BIP0340 | CX_RND_PROVIDED,
        CX_SHA256,
        msg,
        sizeof(msg),
        signature,
        signature_len,
        &info);
}
static void debug_test_case_2(uint8_t* signature) {
    cx_ecfp_private_key_t private_key = {0};
    uint32_t info = 0;
    int sig_len = 0;
    int signature_len = 72;

    // clang-format off
    uint8_t private_key_data[32] = {0xB7, 0xE1, 0x51, 0x62, 0x8A, 0xED, 0x2A, 0x6A,
                                    0xBF, 0x71, 0x58, 0x80, 0x9C, 0xF4, 0xF3, 0xC7,
                                    0x62, 0xE7, 0x16, 0x0F, 0x38, 0xB4, 0xDA, 0x56,
                                    0xA7, 0x84, 0xD9, 0x04, 0x51, 0x90, 0xCF, 0xEF};
    // clang-format off
    uint8_t aux_rand[32] =         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    uint8_t msg[32] =              {0x24, 0x3F, 0x6A, 0x88, 0x85, 0xA3, 0x08, 0xD3,
                                    0x13, 0x19, 0x8A, 0x2E, 0x03, 0x70, 0x73, 0x44,
                                    0xA4, 0x09, 0x38, 0x22, 0x29, 0x9F, 0x31, 0xD0,
                                    0x08, 0x2E, 0xFA, 0x98, 0xEC, 0x4E, 0x6C, 0x89};

    memcpy(signature, aux_rand, sizeof(aux_rand));
    memcpy(private_key.d, private_key_data, sizeof(private_key_data));
    private_key.curve = CX_CURVE_SECP256K1;
    private_key.d_len = sizeof(private_key_data);

    sig_len = cx_ecschnorr_sign(&private_key,
        CX_ECSCHNORR_BIP0340 | CX_RND_PROVIDED,
        CX_SHA256,
        msg,
        sizeof(msg),
        signature,
        signature_len,
        &info);
}

static void debug_test_case_3(uint8_t* signature) {
    cx_ecfp_private_key_t private_key = {0};
    uint32_t info = 0;
    int sig_len = 0;
    int signature_len = 72;

    // clang-format off
    uint8_t private_key_data[32] = {0xC9, 0x0F, 0xDA, 0xA2, 0x21, 0x68, 0xC2, 0x34,
                                    0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1,
                                    0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74,
                                    0x02, 0x0B, 0xBE, 0xA6, 0x3B, 0x14, 0xE5, 0xC9};
    // clang-format off
    uint8_t aux_rand[32] =         {0xC8, 0x7A, 0xA5, 0x38, 0x24, 0xB4, 0xD7, 0xAE,
                                    0x2E, 0xB0, 0x35, 0xA2, 0xB5, 0xBB, 0xBC, 0xCC,
                                    0x08, 0x0E, 0x76, 0xCD, 0xC6, 0xD1, 0x69, 0x2C,
                                    0x4B, 0x0B, 0x62, 0xD7, 0x98, 0xE6, 0xD9, 0x06};
    uint8_t msg[32] =              {0x7E, 0x2D, 0x58, 0xD8, 0xB3, 0xBC, 0xDF, 0x1A,
                                    0xBA, 0xDE, 0xC7, 0x82, 0x90, 0x54, 0xF9, 0x0D,
                                    0xDA, 0x98, 0x05, 0xAA, 0xB5, 0x6C, 0x77, 0x33,
                                    0x30, 0x24, 0xB9, 0xD0, 0xA5, 0x08, 0xB7, 0x5C};

    memcpy(signature, aux_rand, sizeof(aux_rand));
    memcpy(private_key.d, private_key_data, sizeof(private_key_data));
    private_key.curve = CX_CURVE_SECP256K1;
    private_key.d_len = sizeof(private_key_data);

    sig_len = cx_ecschnorr_sign(&private_key,
        CX_ECSCHNORR_BIP0340 | CX_RND_PROVIDED,
        CX_SHA256,
        msg,
        sizeof(msg),
        signature,
        signature_len,
        &info);
}

static void debug_test_case_4(uint8_t* signature) {
    cx_ecfp_private_key_t private_key = {0};
    uint32_t info = 0;
    int sig_len = 0;
    int signature_len = 72;

    // clang-format off
    uint8_t private_key_data[32] = {0x0B, 0x43, 0x2B, 0x26, 0x77, 0x93, 0x73, 0x81,
                                    0xAE, 0xF0, 0x5B, 0xB0, 0x2A, 0x66, 0xEC, 0xD0,
                                    0x12, 0x77, 0x30, 0x62, 0xCF, 0x3F, 0xA2, 0x54,
                                    0x9E, 0x44, 0xF5, 0x8E, 0xD2, 0x40, 0x17, 0x10};
    // clang-format off
    uint8_t aux_rand[32] =         {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t msg[32] =              {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    memcpy(signature, aux_rand, sizeof(aux_rand));
    memcpy(private_key.d, private_key_data, sizeof(private_key_data));
    private_key.curve = CX_CURVE_SECP256K1;
    private_key.d_len = sizeof(private_key_data);

    sig_len = cx_ecschnorr_sign(&private_key,
        CX_ECSCHNORR_BIP0340 | CX_RND_PROVIDED,
        CX_SHA256,
        msg,
        sizeof(msg),
        signature,
        signature_len,
        &info);
}

int handler_debug(int test_case) {
    uint8_t signature[72] = {0};

    switch (test_case) {
        case 1:
            debug_test_case_1(signature);
            break;
        case 2:
            debug_test_case_2(signature);
            break;
        case 3:
            debug_test_case_3(signature);
            break;
        case 4:
            debug_test_case_4(signature);
            break;
        default:
            debug_test_case_1(signature);
    }

    return helper_send_response_sig(signature);
}

#endif