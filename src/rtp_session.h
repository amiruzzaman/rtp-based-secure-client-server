#ifndef RTP_MOD_RTP_SESSION_H
#define RTP_MOD_RTP_SESSION_H

#include "rtp_err.h"
#include "rtp_src.h"
#include <netdb.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>

/**
 * From RFC 3550: An association among a set of participants communicating with
 * RTP.
 */
struct rtp_session {
    size_t n_srcs;
    // sources
    struct rtp_src *srcs;
    uint32_t self_ssrc;
    // TODO: some libevent stuff?
};

struct rtp_session_exchange_ssrc_opts {
    bool async;
};

#define RTP_MOD_SESSION_XCHG_OPT(...) (struct rtp_session_exchange_ssrc_opts) {\
    .async = true,\
    __VA_ARGS__\
}

/**
 * @brief Create a session with random SSRC and zero sources.
 */
struct rtp_session rtp_session_empty_create(unsigned seed);

/**
 * @brief Send `self_ssrc` to the address stored inside `src`.
 * @note If configuration is needed, use `rtp_session_exchange_ssrc_base` and
 * pass in the options.
 */
enum rtp_status rtp_session_exchange_ssrc(struct rtp_src *src);

/**
 * @brief Send `self_ssrc` to the address stored inside `src`, and allows for
 * configuration.
 * @note To pass in options, preferably use the RTP_MOD_SESSION_XCHG_OPT(...)
 * macro, which contains default values.
 */
enum rtp_status rtp_session_exchange_ssrc_base(struct rtp_src *src,
		struct rtp_session_exchange_ssrc_opts opts);

#endif
