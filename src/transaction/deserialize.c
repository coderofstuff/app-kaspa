#include "deserialize.h"
#include "utils.h"
#include "types.h"
#include "../common/buffer.h"

parser_status_e transaction_deserialize(buffer_t *buf, transaction_t *tx) {
    if (buf->size > MAX_TX_LEN) {
        return WRONG_LENGTH_ERROR;
    }

    // nonce
    if (!buffer_read_u64(buf, &tx->nonce, BE)) {
        return NONCE_PARSING_ERROR;
    }

    tx->to = (uint8_t *) (buf->ptr + buf->offset);

    // TO address
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return TO_PARSING_ERROR;
    }

    // amount value
    if (!buffer_read_u64(buf, &tx->value, BE)) {
        return VALUE_PARSING_ERROR;
    }

    // length of memo
    if (!buffer_read_varint(buf, &tx->memo_len) && tx->memo_len > MAX_MEMO_LEN) {
        return MEMO_LENGTH_ERROR;
    }

    // memo
    tx->memo = (uint8_t *) (buf->ptr + buf->offset);

    if (!buffer_seek_cur(buf, tx->memo_len)) {
        return MEMO_PARSING_ERROR;
    }

    if (!transaction_utils_check_encoding(tx->memo, tx->memo_len)) {
        return MEMO_ENCODING_ERROR;
    }

    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}
