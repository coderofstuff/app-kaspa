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
#pragma once

/**
 * Instruction class of the Kaspa application.
 */
#define CLA 0xE0

/**
 * Length of APPNAME variable in the Makefile.
 */
#define APPNAME_LEN (sizeof(APPNAME) - 1)

/**
 * Maximum length of MAJOR_VERSION || MINOR_VERSION || PATCH_VERSION.
 */
#define APPVERSION_LEN 3

/**
 * Maximum length of application name.
 */
#define MAX_APPNAME_LEN 64

/**
 * Maximum message length (bytes) 
 */
#define MAX_MESSAGE_LEN 200

/**
 * Maximum signature length (bytes).
 * Schnorr signatures only have 64 bytes
 */
#define MAX_DER_SIG_LEN 64

/**
 * Exponent used to convert sompi to KAS unit (N KAS = N * 10^8 sompi).
 */
#define EXPONENT_SMALLEST_UNIT 8

/**
 * Normally should be 34 with schnorr addresses but can be 35 with ecdsa addresses
 * Cannot be any longer than 35 in any case
 */
#define MAX_INPUT_SCRIPT_PUBLIC_KEY_LEN 35

/**
 * The signing key used for sighash
 */
#define SIGNING_KEY "TransactionSigningHash"

#define MESSAGE_SIGNING_KEY "PersonalMessageSigningHash"

#define MAX_OUTPUT_COUNT 2
