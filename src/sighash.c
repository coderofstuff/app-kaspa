#include <stdint.h>
#include <string.h>

#include "cx.h"

#include "./transaction/types.h"
#include "./common/buffer.h"
#include "./common/write.h"

const cx_hash_info_t cx_blake2b_info;
const char* signing_key = "TransactionSigningHash";

static cx_err_t cx_blake2b_init3_no_throw(cx_blake2b_t* hash,
                                          size_t size,
                                          uint8_t* key,
                                          size_t key_len) {
    if (key == NULL && key_len != 0) {
        goto err;
    }

    if (size % 8 != 0 || size < 8 || size > 512) {
        goto err;
    }
    memset(hash, 0, sizeof(cx_blake2b_t));

    size = size / 8;
    hash->output_size = size;
    hash->header.info = &cx_blake2b_info;

    if (blake2b_init_key(&hash->ctx, size, key, key_len) < 0) {
        goto err;
    }
    return CX_OK;

err:
    return CX_INVALID_PARAMETER;
}

static void calc_prev_outputs_hash(transaction_t* tx, uint8_t* out_hash) {
    cx_blake2b_t inner_hash_writer;
    cx_blake2b_init3_no_throw(&inner_hash_writer, 256, (uint8_t*) signing_key, 22);

    for (size_t i = 0; i < tx->tx_input_len; i++) {
        uint8_t output_index_le[4] = {tx->tx_inputs[i].index, 0x00, 0x00, 0x00};

        cx_blake2b_update(&inner_hash_writer, tx->tx_inputs[i].tx_id, 32);
        cx_blake2b_update(&inner_hash_writer, output_index_le, 4);
    }

    cx_blake2b_final(&inner_hash_writer, out_hash);
}

static void calc_sequences_hash(transaction_t* tx, uint8_t* out_hash) {
    cx_blake2b_t inner_hash_writer;
    cx_blake2b_init3_no_throw(&inner_hash_writer, 256, (uint8_t*) signing_key, 22);

    uint8_t curr_sequences_hash[8] = {0};

    for (size_t i = 0; i < tx->tx_input_len; i++) {
        write_u64_le(curr_sequences_hash, 0, tx->tx_inputs[i].sequence);
        cx_blake2b_update(&inner_hash_writer, curr_sequences_hash, 8);
        memset(curr_sequences_hash, 0, sizeof(curr_sequences_hash));
    }

    cx_blake2b_final(&inner_hash_writer, out_hash);
}

static void calc_sig_op_count_hash(transaction_t* tx, uint8_t* out_hash) {
    cx_blake2b_t inner_hash_writer;
    cx_blake2b_init3_no_throw(&inner_hash_writer, 256, (uint8_t*) signing_key, 22);

    for (size_t i = 0; i < tx->tx_input_len; i++) {
        uint8_t dummy_sig_op_count[1] = {0x01};

        cx_blake2b_update(&inner_hash_writer, dummy_sig_op_count, 1);
    }

    cx_blake2b_final(&inner_hash_writer, out_hash);
}

static void calc_outputs_hash(transaction_t* tx, uint8_t* out_hash) {
    cx_blake2b_t inner_hash_writer;
    cx_blake2b_init3_no_throw(&inner_hash_writer, 256, (uint8_t*) signing_key, 22);

    uint8_t buf[8] = {0};

    for (size_t i = 0; i < tx->tx_output_len; i++) {
        memset(buf, 0, sizeof(buf));

        write_u64_le(buf, 0, tx->tx_outputs[i].value);
        cx_blake2b_update(&inner_hash_writer, buf, 8);  // Write the output value
        memset(buf, 0, sizeof(buf));

        cx_blake2b_update(&inner_hash_writer, buf, 2);  // Write the output script version, assume 0

        // First byte is always the length of the following public key
        // Last byte is always 0xac (op code for normal transactions)
        uint8_t script_len = tx->tx_outputs[i].script_public_key[0] + 2;
        write_u64_le(buf, 0, script_len);  // Write the number of bytes of the script public key
        cx_blake2b_update(&inner_hash_writer, buf, 8);
        cx_blake2b_update(&inner_hash_writer, tx->tx_outputs[i].script_public_key, script_len);
    }

    cx_blake2b_final(&inner_hash_writer, out_hash);
}

void calc_sighash(transaction_t* tx, transaction_input_t* txin, uint8_t* out_hash) {
    uint8_t buf[32] = {0};

    cx_blake2b_t sighash;

    cx_blake2b_init3_no_throw(&sighash, 256, (uint8_t*) signing_key, 22);

    // Write version, little endian, 2 bytes
    write_u16_le(buf, 0, tx->version);
    cx_blake2b_update(&sighash, buf, 2);
    memset(buf, 0, sizeof(buf));

    // Write previous outputs hash
    calc_prev_outputs_hash(tx, buf);
    cx_blake2b_update(&sighash, buf, 32);
    memset(buf, 0, sizeof(buf));

    // Write sequence hash
    calc_sequences_hash(tx, buf);
    cx_blake2b_update(&sighash, buf, 32);
    memset(buf, 0, sizeof(buf));

    // Write sig op count hash
    calc_sig_op_count_hash(tx, buf);
    cx_blake2b_update(&sighash, buf, 32);
    memset(buf, 0, sizeof(buf));

    // Write Hash of the outpoint
    cx_blake2b_update(&sighash, txin->tx_id, 32);
    write_u32_le(buf, 0, txin->index);
    cx_blake2b_update(&sighash, buf, 4);
    memset(buf, 0, sizeof(buf));

    cx_blake2b_update(&sighash, buf, 2);  // Write input script version, assume 0

    // Write input's script_public_key. Length as uint64_t followed by script
    // count (1 byte) + public key (32/33 byte) + op (1 byte)
    uint8_t script_len = txin->script_public_key[0] + 2;
    write_u64_le(buf, 0, script_len);
    cx_blake2b_update(&sighash, buf, 8);
    cx_blake2b_update(&sighash, txin->script_public_key, script_len);
    memset(buf, 0, sizeof(buf));

    // Write input's value
    write_u64_le(buf, 0, txin->value);
    cx_blake2b_update(&sighash, buf, 8);
    memset(buf, 0, sizeof(buf));

    // Write input's sequence number
    write_u64_le(buf, 0, txin->sequence);
    cx_blake2b_update(&sighash, buf, 8);
    memset(buf, 0, sizeof(buf));

    // Write sigopcount, assume 1
    buf[0] = 0x01;
    cx_blake2b_update(&sighash, buf, 1);
    memset(buf, 0, sizeof(buf));

    // Write outputs hash
    calc_outputs_hash(tx, buf);
    cx_blake2b_update(&sighash, buf, 32);
    memset(buf, 0, sizeof(buf));

    // Write last bits of data, assuming 0
    cx_blake2b_update(&sighash, buf, 8);   // Write locktime of 0
    cx_blake2b_update(&sighash, buf, 20);  // Write subnetwork Id, assume zero hash
    cx_blake2b_update(&sighash, buf, 8);   // Write gas, assume 0
    cx_blake2b_update(&sighash, buf, 32);  // Write payload hash, assume 0

    // Write sighash type, assume SigHashAll => 0x01
    buf[0] = 0x01;
    cx_blake2b_update(&sighash, buf, 1);

    cx_blake2b_final(&sighash, out_hash);
}
