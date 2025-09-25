#include "rtp_util.h"
#include <stdlib.h>

size_t rtp_util_bits_to_bytes(size_t bits) { return (bits + 7) / 8; }

size_t rtp_util_bytes_to_bits(size_t bytes) { return bytes * 8; }
