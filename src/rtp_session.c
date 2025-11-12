#include "rtp_session.h"
#include <stdlib.h>

struct rtp_session rtp_session_empty_create(unsigned seed) {
    srand(seed);
    return (struct rtp_session) {
        .n_srcs = 0,
	.srcs = nullptr,
	// rand ranges in [0, RAND_MAX]
	.self_ssrc = (uint32_t)rand(),
    };
}
