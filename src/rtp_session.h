#ifndef RTP_MOD_RTP_SESSION_H
#define RTP_MOD_RTP_SESSION_H

#include "rtp_err.h"
#include <stddef.h>
#include <time.h>

struct rtp_src_data;

struct rtp_session {
    size_t n_srcs;
    struct rtp_src_data *srcs;
    uint32_t self_ssrc;
    // timeout in seconds. Default to 69.
    uint8_t timeout_secs;
    // each session should only send 1 type of payload, because different
    // payload types might have different timing increments.
    uint8_t payload_type : 7;
};

/**
 * @brief Generate random SSRC and assign the supplied payload type. That's
 * all. Default timeout is 69 seconds.
 * @note For the first argument `session`, simply zero-initialize an
 * `rtp_session` and pass that in. This function does not allocate a session
 * but only writes data to one.
 * @note For a good default seed, just get clock time (say, `clock_gettime`).
 * @note Our convention is, `session`, if successfully created, now owns
 * `self_addr`.
 * @note Payload type should fit into 7 bits (that is, its value should not be
 * larger than 127).
 * @return STATUS_OK on success. And, for now, it cannot fail, but there might
 * be some changes in the future that might create a fail condition.
 */
enum rtp_status rtp_session_create(struct rtp_session *session, unsigned seed,
                                   uint8_t payload_type);

/**
 * @brief Like @ref rtp_session_create, but instead of randomly generating an
 * SSRC, just use the supplied `ssrc`.
 */
enum rtp_status rtp_session_create_with_ssrc(struct rtp_session *session,
                                             uint16_t ssrc,
                                             uint8_t payload_type);

/**
 * @brief Add a source. Simple as that.
 * @todo What if there's already a source with the specified SSRC?
 * @note You'd probably want to get the source data using another protocol,
 * like SIP.
 */
enum rtp_status rtp_session_add_src(struct rtp_session *session,
                                    struct rtp_src_data *data);

/**
 * @brief Self-explanatory. Run it when you're done with your session.
 * @note This function doesn't assume `session` is dynamically allocated - if
 * `session` was dynamically allocated, remember to free it.
 */
void rtp_session_nuke(struct rtp_session *session);

#endif
