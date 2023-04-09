#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool
#include <stddef.h>   // size_t
#include <string.h>   // memmove

#include "serialize.h"
#include "../common/write.h"
#include "../common/varint.h"

int transaction_serialize(const transaction_t *tx, uint8_t *out, size_t out_len) {
    size_t offset = 0;

    // if (8 + ADDRESS_LEN + 8 + varint_size(tx->memo_len) + tx->memo_len > out_len) {
    //     return -1;
    // }

    // // nonce
    // write_u64_be(out, offset, tx->nonce);
    // offset += 8;

    // // to
    // memmove(out + offset, tx->to, ADDRESS_LEN);
    // offset += ADDRESS_LEN;

    // // value
    // write_u64_be(out, offset, tx->value);
    // offset += 8;

    // // memo length
    // int varint_len = varint_write(out, offset, tx->memo_len);
    // if (varint_len < 0) {
    //     return -1;
    // }
    // offset += varint_len;

    // // memo
    // memmove(out + offset, tx->memo, tx->memo_len);
    // offset += tx->memo_len;

    return (int) offset;
}
