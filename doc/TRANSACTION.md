# Kaspa Transaction Serialization

## Overview

The custom transaction serialization presented is for Kaspa.

## Amount units

The base unit in Kaspa is the KAS and the smallest unit used in raw transaction is the *sompi*: 1 KAS = 100000000 sompi.

## Address format

Kaspa addresses begin with `kaspa:` followed by 61 base32 characters for a total of `67` bytes.

## Structure

### Transaction

| Field | Size (bytes) | Description |
| --- | :---: | --- |
| `version` | 2 | The version of the transaction being signed |
| `n_inputs` | 1 | The number of inputs. Max 255.
| `inputs[]` | n_inputs * 69 | Array of `Transaction Inputs` |
| `n_outputs` | 1 | The number of outputs. Max 255.
| `outputs[]` | n_outputs * 42 | Array of `Transaction Outputs` |

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
| `script_public_key` | 35 | Schnorr: `20` + public_key (32 bytes) + `ac` <br/> ECDSA: `20` + public_key (33 bytes) + `ab` |

### Transaction Requirements
- Fee = (total inputs amount) - (total outputs amount)
- (total inputs amount) > (total outputs amount)
- There must be at least 1 input
- There must be exactly 1 or 2 outputs
  - If there is only 1 output, it is assumed to be the send address
  - If there are 2 outputs, the first output is assumed to be the `send` address and the second output is where the `change` will go

### Signature

[Schnorr](https://github.com/bitcoin/bips/blob/master/bip-0340.mediawiki) is used to sign transaction on the [SECP-256k1](https://www.secg.org/sec2-v2.pdf#subsubsection.2.4.1) curve.

It is used to sign the [sighash](https://kaspa-mdbook.aspectron.com/transactions/sighashes.html).

### Fee

The fee is the difference between the sum of input values and the sum of the output values.

## Links

- Bitcoin Transaction: https://kaspa-mdbook.aspectron.com/transactions.html
