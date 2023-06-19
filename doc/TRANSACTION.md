# Kaspa Transaction Serialization

## Overview

The custom transaction serialization presented is for Kaspa.

## Amount units

The base unit in Kaspa is the KAS and the smallest unit used in raw transaction is the *sompi*: 1 KAS = 100000000 sompi.

## Address format

Kaspa addresses begin with `kaspa:` followed by 61 base32 characters for a total of `67` bytes for Schnorr-signed and P2SH addresses. P2SH addresses are supported only as a send address by this app.

For ECDSA-signed addresses (supported by this app only as a send address), it begins with `kaspa:` followed by 63 bytes for a total of `69` bytes.

## Structure

### Transaction

| Field | Size (bytes) | Description |
| --- | :---: | --- |
| `version` | 2 | The version of the transaction being signed |
| `n_inputs` | 1 | The number of inputs. Max 255.
| `n_outputs` | 1 | The number of outputs. Exactly 1 or 2.
| `change_address_type` | 1 | `0` if `RECEIVE` or `1` if `CHANGE`* |
| `change_address_index` | 4 | `0x00000000` to `0xFFFFFFFF`**|

\* While this will be used for the change, the path may be either `RECEIVE` or `CHANGE`.
This is necessary in case the user wants to send the change back to the same address.
In this case, the `change_address_type` has to be set to `RECEIVE`.

\*\* `change_address_type` and `change_address_index` are ignored if `n_outputs == 1`. If `n_outputs == 2` then the path defined here must resolve to the same `script_public_key` in `outputs[1]`.
### Transaction Input

Total bytes: 46

| Field | Size (bytes) | Description |
| --- | --- | --- |
| `address_type` | 1 | 0x00 for RECEIVE or 0x01 for CHANGE address |
| `address_index` | 4 | The index of this address in the derivation path |
| `value` | 8 | The amount of KAS in sompi in this input |
| `index` | 1 | The index of this outpoint |
| `prev_tx_id` | 32 | The transaction ID in bytes |
<!--
| `sequence` | 8 | The sequence number of this input |
| `sig_op_count` | 1 | The sig op count. Usually `1` |
-->

### Transaction Output

Total bytes: 43 (max)

| Field | Size (bytes) | Description |
| --- | --- | --- |
| `value` | 8 | The amount of KAS in sompi that will go send to the address |
| `script_public_key` | 35 | Schnorr: `20` + public_key (32 bytes) + `ac` <br/> ECDSA: `20` + public_key (33 bytes) + `ab` <br/> P2SH: `aa20` + public_key (32 bytes) + `87` |

### Transaction Requirements
- Fee = (total inputs amount) - (total outputs amount)
- (total inputs amount) > (total outputs amount)
- There must be at least 1 input
- There must be exactly 1 or 2 outputs
  - If there is only 1 output, it is assumed to be the `send` address
  - If there are 2 outputs, the first output is assumed to be the `send` address and the second output is where the `change` will go
    - The `script_public_key` for the change must resolve to the same value that the change address type and index resolve to. This is validated in the ledger device.

### Signature

[Schnorr](https://github.com/bitcoin/bips/blob/master/bip-0340.mediawiki) is used to sign transaction on the [SECP-256k1](https://www.secg.org/sec2-v2.pdf#subsubsection.2.4.1) curve.

It is used to sign the [SigHash](https://kaspa-mdbook.aspectron.com/transactions/sighashes.html).

### Fee

The fee is the difference between the sum of input values and the sum of the output values.

## Links

- Kaspa Transaction: https://kaspa-mdbook.aspectron.com/transactions.html
