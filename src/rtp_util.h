#ifndef RTP_MOD_RTP_UTIL_H
#define RTP_MOD_RTP_UTIL_H

#include <stdlib.h>

// sometimes i do miss C++'s constexpr
#define RTP_UTIL_BITS_TO_BYTES(bits) \
    (((bits) + 7) / 8)

#define RTP_UTIL_BYTES_TO_BITS(bytes) \
    ((bytes) * 8)

#endif
