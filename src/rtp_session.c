#include "rtp_session.h"
#include "rtp_err.h"
#include <assert.h>
#include <event2/event.h>
#include <stdlib.h>

struct rtp_data_buffer {
    size_t len;
    uint8_t *buff;
};

struct rtp_write_event_args {
    struct rtp_src_data *self;
    const struct rtp_data_buffer *write_buffer;
};

struct rtp_read_event_args {
    struct rtp_src_data *self;
    struct rtp_data_buffer *read_buffer;
};

struct rtp_src_data {
    // either IPv4 or IPv6.
    struct sockaddr_storage addr;
    evutil_socket_t sock;
    struct event_base *ev_base;
    struct event *ev_read;
    struct event *ev_write;
    struct rtp_read_event_args ev_read_args;
    struct rtp_write_event_args ev_write_args;
    // seq number is only 2 bytes long, so it will wrap around, unless the
    // session is very, very short.
    // Any time the sequence wraps around, increment by (largest seq num
    // possible + 1) (that is, `UINT16_MAX + 1`)
    size_t seq_cycles;
    uint32_t ssrc;
    uint16_t max_seq;
    uint16_t base_seq;
    // probably capped at 64K (65535 bytes or something)
    uint16_t max_pack_size;
};

enum rtp_status rtp_session_create(struct rtp_session *session, unsigned seed,
                                   uint8_t payload_type) {
    assert(session != nullptr);
    assert(payload_type < 128);
    srand(seed);
    return rtp_session_create_with_ssrc(session, (uint16_t)rand(),
                                        payload_type);
}

enum rtp_status rtp_session_create_with_ssrc(struct rtp_session *session,
                                             uint16_t ssrc,
                                             uint8_t payload_type) {
    assert(session != nullptr);
    assert(payload_type < 128);
    enum rtp_status ret = STATUS_OK;

    session->n_srcs = 0;
    session->srcs = nullptr;
    session->self_ssrc = ssrc;
    session->payload_type = payload_type;

    return ret;
}
