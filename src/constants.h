#pragma once

/**
 * FIXME - change to kaspa's own CLA. This one is from Boilerplate
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
 * Maximum transaction length (bytes).
 */
#define MAX_TRANSACTION_LEN 512

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
