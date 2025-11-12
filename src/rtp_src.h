#ifndef RTP_MOD_RTP_SRC_H
#define RTP_MOD_RTP_SRC_H

#include "rtp_err.h"
#include <stdint.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

/**
 * A participant, basically.
 */
struct rtp_src {
    // could be IPv4, could be IPv6, we dunno.
    // So, we use sockaddr_storage.
    struct sockaddr *addr;
    uint32_t ssrc;
    // TODO: keep track of some more info (e.g. number of packets received from
    // this source).
};

#endif
