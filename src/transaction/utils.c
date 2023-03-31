#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool
#include <string.h>   // memmove

#include "types.h"

bool transaction_utils_check_encoding(const uint8_t *memo, uint64_t memo_len) {
    for (uint64_t i = 0; i < memo_len; i++) {
        if (memo[i] > 0x7F) {
            return false;
        }
    }

    return true;
}

bool transaction_utils_format_memo(const uint8_t *memo,
                                   uint64_t memo_len,
                                   char *dst,
                                   uint64_t dst_len) {
    if (memo_len > MAX_MEMO_LEN || dst_len < memo_len + 1) {
        return false;
    }

    memmove(dst, memo, memo_len);
    dst[memo_len] = '\0';

    return true;
}
