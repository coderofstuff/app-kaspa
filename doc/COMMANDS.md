# Kaspa commands

## Overview

| Command name | INS | Description |
| --- | --- | --- |
| `GET_VERSION` | 0x03 | Get application version as `MAJOR`, `MINOR`, `PATCH` |
| `GET_APP_NAME` | 0x04 | Get ASCII encoded application name |
| `GET_PUBLIC_KEY` | 0x05 | Get public key given BIP32 path |
| `SIGN_TX` | 0x06 | Sign transaction given BIP32 path and raw transaction |

## GET_VERSION

### Command

| CLA | INS | P1 | P2 | Lc | CData |
| --- | --- | --- | --- | --- | --- |
| 0xE0 | 0x03 | 0x00 | 0x00 | 0x00 | - |

### Response

| Response length (bytes) | SW | RData |
| --- | --- | --- |
| 3 | 0x9000 | `MAJOR (1)` \|\| `MINOR (1)` \|\| `PATCH (1)` |

## GET_APP_NAME

### Command

| CLA | INS | P1 | P2 | Lc | CData |
| --- | --- | --- | --- | --- | --- |
| 0xE0 | 0x04 | 0x00 | 0x00 | 0x00 | - |

### Response

| Response length (bytes) | SW | RData |
| --- | --- | --- |
| 5 | 0x9000 | `Kaspa (5 bytes)` |

Raw response looks like: `4b617370619000`

## GET_PUBLIC_KEY

### Command

| CLA | INS | P1 | P2 | Lc | CData |
| --- | --- | --- | --- | --- | --- |
| 0xE0 | 0x05 | 0x00 (no display) <br> 0x01 (display) | 0x00 | 0x15 | `path len (1 byte)` \|\|<br>`purpose (4 bytes)` \|\|<br> `coin_type (4 bytes)` \|\|<br> `account (4 bytes)` \|\|<br> `type (4 bytes)` \|\|<br>`index (4 bytes)` |

Keys for kaspa use the derivation path `m/44'/111111'/<account>'/<type>/<index>`. As such you only need to pass the `type` and `index` here.

| CData Part | Description |
| --- | --- |
| `purpose` | Must be `44'` or `80000002c` |
| `coin_type` | Must be `111111'` or `8001b207` |
| `account` | Current wallets all use `80000000` for `0'` for default account but any value is accepted |
| `type` | Either `00000000` for Receive Address or `00000001` for Change Address |
| `index` | Any value from `00000000` to `11111111` |

### Response

| Response length (bytes) | SW | RData |
| --- | --- | --- |
| var | 0x9000 | `len(public_key) (1)` \|\|<br> `public_key (65 bytes)` \|\|<br> `len(chain_code) (1)` \|\|<br> `chain_code (var)` |

`public_key` here will always be `65 bytes`, represending the uncompressed public key in DER encoding.
It is composed of `0x04` followed by `32 bytes` for the `X` coordinate of the public key, then `32 bytes` for the `Y` coordinate.

In kaspa, you only need the `32 bytes` for the `X` coordinate as the public key to generate addresses whose transactions are signed with Schnorr.

Transactions signed with ECDSA are currently not supported.

## SIGN_TX

### Command

| CLA | INS | P1 | P2 | Lc | CData |
| --- | --- | --- | --- | --- | --- |
| 0xE0 | 0x06 | 0x00-0x03 (chunk index) | 0x00 (more) <br> 0x80 (last) | 1 + 4n | `len(bip32_path) (1)` \|\|<br> `bip32_path{1} (4)` \|\|<br>`...` \|\|<br>`bip32_path{n} (4)` |

### Response

| Response length (bytes) | SW | RData |
| --- | --- | --- |
| var | 0x9000 | `len(signature_count) (1)` \|\| <br> `len(signature{1})` \|\| <br> `signature{n} (var)` \|\| <br> ... \|\| <br> `len(signature{n})` \|\| <br> `signature{n} (var)` |

## Status Words

| SW | SW name | Description |
| --- | --- | --- |
| 0x6985 | `SW_DENY` | Rejected by user |
| 0x6A86 | `SW_WRONG_P1P2` | Either `P1` or `P2` is incorrect |
| 0x6A87 | `SW_WRONG_DATA_LENGTH` | `Lc` or minimum APDU length is incorrect |
| 0x6D00 | `SW_INS_NOT_SUPPORTED` | No command exists with `INS` |
| 0x6E00 | `SW_CLA_NOT_SUPPORTED` | Bad `CLA` used for this application |
| 0xB000 | `SW_WRONG_RESPONSE_LENGTH` | Wrong response length (buffer size problem) |
| 0xB001 | `SW_DISPLAY_BIP32_PATH_FAIL` | BIP32 path conversion to string failed |
| 0xB002 | `SW_DISPLAY_ADDRESS_FAIL` | Address conversion to string failed |
| 0xB003 | `SW_DISPLAY_AMOUNT_FAIL` | Amount conversion to string failed |
| 0xB004 | `SW_WRONG_TX_LENGTH` | Wrong raw transaction length |
| 0xB005 | `SW_TX_PARSING_FAIL` | Failed to parse raw transaction |
| 0xB006 | `SW_TX_HASH_FAIL` | Failed to compute hash digest of raw transaction |
| 0xB007 | `SW_BAD_STATE` | Security issue with bad state |
| 0xB008 | `SW_SIGNATURE_FAIL` | Signature of raw transaction failed |
| 0xB009 | `SW_WRONG_BIP32_PURPOSE` | `Purpose` must be `44'` |
| 0xB00A | `SW_WRONG_BIP32_COIN_TYPE` | `Coin Type` must be `111111'` |
| 0xB00B | `SW_WRONG_BIP32_TYPE` | `Type` passed is not valid. Must be either `0`  for `Receive` or `1`  for `Change`|
| 0x9000 | `OK` | Success |
