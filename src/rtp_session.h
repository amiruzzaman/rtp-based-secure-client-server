#ifndef RTP_MOD_RTP_SESSION_H
#define RTP_MOD_RTP_SESSION_H

#include "rtp_err.h"
#include <event2/event.h>
#include <netdb.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

struct rtp_src_data {
    // either IPv4 or IPv6.
    struct sockaddr_storage addr;
    // seq number is only 2 bytes long, so it will wrap around, unless the
    // session is very, very short.
    // Any time the sequence wraps around, increment by (largest seq num
    // possible + 1) (that is, `UINT16_MAX + 1`)
    size_t seq_cycles;
    uint32_t ssrc;
    uint16_t max_seq;
    uint16_t base_seq;
};

struct rtp_data_buffer {
    size_t len;
    uint8_t buff[];
};

struct rtp_write_event_args {
    struct rtp_session *self;
    const struct rtp_data_buffer *write_buffer;
    struct rtp_src_data *send_to_src_data;
};

struct rtp_read_event_args {
    struct rtp_session *self;
    struct rtp_data_buffer *read_buffer;
};

struct rtp_session {
    // where we will send our packets to. Can be IPv4 or 6.
    struct sockaddr_storage self_addr;
    struct rtp_write_event_args write_ev_args;
    struct rtp_read_event_args read_ev_args;
    size_t n_srcs;
    struct rtp_src_data *srcs;
    uint32_t self_ssrc;
    evutil_socket_t sock;
    // each session should only send 1 type of payload, because different
    // payload types might have different timing increments.
    uint8_t payload_type : 7;

    struct event_base *ev_base;
    struct event *read_event;
    struct event *write_event;
};

/**
 * @brief Generate random SSRC, store `self_addr`, create a socket and some
 * event handlers to deal with data in/out of that socket.
 * @note For the first argument `session`, simply zero-initialize an
 * `rtp_session` and pass that in. This function does not allocate a session
 * but only writes data to one.
 * @return STATUS_OK on success. On failure: TODO what do we return?
 */
enum rtp_status rtp_session_create(struct rtp_session *session, unsigned seed,
                                   struct sockaddr *self_addr);
/**
 * @brief Self-explanatory. Run it when you're done with your session.
 * @note This function doesn't assume `session` is dynamically allocated - if
 * `session` was dynamically allocated, remember to free it.
 */
void rtp_session_nuke(struct rtp_session *session);

#endif
