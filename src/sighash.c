#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "./import/blake2-impl.h"
#include "./import/blake2b.h"

#include "./transaction/types.h"
#include "./common/buffer.h"
#include "./common/write.h"
#include "globals.h"
#include "./constants.h"

static int hash_init(blake2b_state* hash, size_t size, uint8_t* key, size_t key_len) {
    if (key == NULL && key_len != 0) {
        goto err;
    }

    if (size % 8 != 0 || size < 8 || size > 512) {
        goto err;
    }
    memset(hash, 0, sizeof(blake2b_state));

    size = size / 8;

    if (blake2b_init_key(hash, size, key, key_len) < 0) {
        goto err;
    }
    return 0;

err:
    return -1;
}

static void hash_update(blake2b_state* hash, uint8_t* data, size_t len) {
    blake2b_update(hash, data, len);
}

static void hash_finalize(blake2b_state* hash, uint8_t* out) {
    blake2b_final(hash, out, 32);
}

static void calc_prev_outputs_hash(transaction_t* tx, uint8_t* out_hash) {
    uint8_t inner_buffer[32] = {0};
    blake2b_state inner_hash_writer;
    hash_init(&inner_hash_writer, 256, (uint8_t*) SIGNING_KEY, 22);

    for (size_t i = 0; i < tx->tx_input_len; i++) {
        memset(inner_buffer, 0, sizeof(inner_buffer));
        write_u32_le(inner_buffer, 0, tx->tx_inputs[i].index);
        hash_update(&inner_hash_writer, tx->tx_inputs[i].tx_id, 32);
        hash_update(&inner_hash_writer, inner_buffer, 4);
    }

    hash_finalize(&inner_hash_writer, out_hash);
}

static void calc_sequences_hash(transaction_t* tx, uint8_t* out_hash) {
    uint8_t inner_buffer[32] = {0};
    blake2b_state inner_hash_writer;
    hash_init(&inner_hash_writer, 256, (uint8_t*) SIGNING_KEY, 22);

    for (size_t i = 0; i < tx->tx_input_len; i++) {
        memset(inner_buffer, 0, sizeof(inner_buffer));
        write_u64_le(inner_buffer, 0, tx->tx_inputs[i].sequence);
        hash_update(&inner_hash_writer, inner_buffer, 8);
        memset(inner_buffer, 0, sizeof(inner_buffer));
    }

    hash_finalize(&inner_hash_writer, out_hash);
}

static void calc_sig_op_count_hash(transaction_t* tx, uint8_t* out_hash) {
    uint8_t inner_buffer[32] = {0};
    blake2b_state inner_hash_writer;
    hash_init(&inner_hash_writer, 256, (uint8_t*) SIGNING_KEY, 22);

    for (size_t i = 0; i < tx->tx_input_len; i++) {
        memset(inner_buffer, 1, 1);
        hash_update(&inner_hash_writer, inner_buffer, 1);
        memset(inner_buffer, 0, sizeof(inner_buffer));
    }

    hash_finalize(&inner_hash_writer, out_hash);
}

static void calc_outputs_hash(transaction_t* tx, uint8_t* out_hash) {
    uint8_t inner_buffer[32] = {0};
    blake2b_state inner_hash_writer;
    hash_init(&inner_hash_writer, 256, (uint8_t*) SIGNING_KEY, 22);

    for (size_t i = 0; i < tx->tx_output_len; i++) {
        memset(inner_buffer, 0, sizeof(inner_buffer));

        write_u64_le(inner_buffer, 0, tx->tx_outputs[i].value);
        hash_update(&inner_hash_writer, inner_buffer, 8);  // Write the output value
        memset(inner_buffer, 0, sizeof(inner_buffer));

        hash_update(&inner_hash_writer,
                    inner_buffer,
                    2);  // Write the output script version, assume 0

        // First byte is always the length of the following public key
        // Last byte is always 0xac (op code for normal transactions)
        uint8_t script_len = tx->tx_outputs[i].script_public_key[0] + 2;
        write_u64_le(inner_buffer,
                     0,
                     script_len);  // Write the number of bytes of the script public key
        hash_update(&inner_hash_writer, inner_buffer, 8);
        hash_update(&inner_hash_writer, tx->tx_outputs[i].script_public_key, script_len);
    }

    hash_finalize(&inner_hash_writer, out_hash);
}

static bool calc_txin_script_public_key(uint8_t* public_key, uint8_t* out_hash) {
    // Assume schnorr
    out_hash[0] = 0x20;
    memmove(out_hash + 1, public_key, 32);
    out_hash[33] = OP_CHECKSIG;

    return true;
}

void calc_sighash(transaction_t* tx,
                  transaction_input_t* txin,
                  uint8_t* public_key,
                  uint8_t* out_hash) {
    // Buffer needs to come before the state
    uint8_t outer_buffer[32] = {0};
    blake2b_state sighash;
    hash_init(&sighash, 256, (uint8_t*) SIGNING_KEY, 22);

    // Write version, little endian, 2 bytes
    write_u16_le(outer_buffer, 0, tx->version);
    hash_update(&sighash, outer_buffer, 2);
    memset(outer_buffer, 0, sizeof(outer_buffer));

    // Write previous outputs hash
    calc_prev_outputs_hash(tx, outer_buffer);
    hash_update(&sighash, outer_buffer, 32);
    memset(outer_buffer, 0, sizeof(outer_buffer));

    // Write sequence hash
    calc_sequences_hash(tx, outer_buffer);
    hash_update(&sighash, outer_buffer, 32);
    memset(outer_buffer, 0, sizeof(outer_buffer));

    // Write sig op count hash
    calc_sig_op_count_hash(tx, outer_buffer);
    hash_update(&sighash, outer_buffer, 32);
    memset(outer_buffer, 0, sizeof(outer_buffer));

    // Write Hash of the outpoint
    hash_update(&sighash, txin->tx_id, 32);
    write_u32_le(outer_buffer, 0, txin->index);
    hash_update(&sighash, outer_buffer, 4);
    memset(outer_buffer, 0, sizeof(outer_buffer));

    hash_update(&sighash, outer_buffer, 2);  // Write input script version, assume 0

    // Write input's script_public_key. Length as uint64_t followed by script
    // count (1 byte) + public key (32/33 byte) + op (1 byte)
    uint64_t script_len = 34;
    write_u64_le(outer_buffer, 0, script_len);
    hash_update(&sighash, outer_buffer, 8);

    calc_txin_script_public_key(public_key, outer_buffer);
    hash_update(&sighash, outer_buffer, script_len);
    memset(outer_buffer, 0, sizeof(outer_buffer));

    // Write input's value
    write_u64_le(outer_buffer, 0, txin->value);
    hash_update(&sighash, outer_buffer, 8);
    memset(outer_buffer, 0, sizeof(outer_buffer));

    // Write input's sequence number
    write_u64_le(outer_buffer, 0, txin->sequence);
    hash_update(&sighash, outer_buffer, 8);
    memset(outer_buffer, 0, sizeof(outer_buffer));

    // Write sigopcount, assume 1
    outer_buffer[0] = 0x01;
    hash_update(&sighash, outer_buffer, 1);
    memset(outer_buffer, 0, sizeof(outer_buffer));

    // Write outputs hash
    calc_outputs_hash(tx, outer_buffer);
    hash_update(&sighash, outer_buffer, 32);
    memset(outer_buffer, 0, sizeof(outer_buffer));

    // Write last bits of data, assuming 0
    hash_update(&sighash, outer_buffer, 8);   // Write locktime of 0
    hash_update(&sighash, outer_buffer, 20);  // Write subnetwork Id, assume zero hash
    hash_update(&sighash, outer_buffer, 8);   // Write gas, assume 0
    hash_update(&sighash, outer_buffer, 32);  // Write payload hash, assume 0

    // Write sighash type, assume SigHashAll => 0x01
    outer_buffer[0] = 0x01;
    hash_update(&sighash, outer_buffer, 1);

    hash_finalize(&sighash, out_hash);
}
