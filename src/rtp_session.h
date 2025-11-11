#ifndef RTP_MOD_RTP_SESSION_H
#define RTP_MOD_RTP_SESSION_H

#include <stddef.h>
#include <stdint.h>

/**
 * A participant, basically.
 */
struct rtp_src {
    uint32_t ssrc;
};

/**
 * From RFC 3550: An association among a set of participants communicating with
 * RTP.
 */
struct rtp_session {};

#endif
