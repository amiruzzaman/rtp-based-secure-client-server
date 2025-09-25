#ifndef RTP_MOD_RTP_UTIL_H
#define RTP_MOD_RTP_UTIL_H

#include <stdlib.h>

// round-up convert bits to bytes
size_t rtp_util_bits_to_bytes(size_t bits);
// literally just multiply by 8.
size_t rtp_util_bytes_to_bits(size_t bytes);

#endif
