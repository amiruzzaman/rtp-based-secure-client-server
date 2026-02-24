#ifndef RTP_MOD_RTP_SESSION_H
#define RTP_MOD_RTP_SESSION_H

// TODO: start writing RTP session once I'm done with RTCP.

#include "rtp_err.h"
#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>

/**
 * \brief A peer exchanges data with the current host device.
 *
 * A host can appear as a peer in each different session.
 */
struct rtp_peer;
/**
 * \brief A session handles one specific type of stream from one or more peers.
 */
struct rtp_session;

/**
 * \brief Initializes a peer using the provided \p addr.
 * \param peer The peer to initialize. Must not be \c nullptr.
 * \param addr The address of this peer.
 * \return \c STATUS_OK if successful, other errors defined in \ref rtp_status
 * otherwise.
 * \warning \p session must not be \c nullptr.
 */
enum rtp_status rtp_peer_init_from_addr(struct rtp_peer *peer,
                                        struct sockaddr_storage addr);
/**
 * \brief Initializes an empty session.
 */
enum rtp_status rtp_session_init(struct rtp_session *session);
/**
 * \brief Sets the provided \p session.
 * \param session The session whose payload type is to be set.
 * \param payload_type The payload type to set to.
 * \warning \p session must not be \c nullptr.
 */
enum rtp_status rtp_session_set_payload_type(struct rtp_session *session,
                                             uin8_t payload_type);
/**
 * \brief Gets the \p session payload type.
 * \param session The session to get the payload type from.
 * \return The payload type.
 * \warning \p session must not be \c nullptr.
 */
uint8_t rtp_session_get_payload_type(const struct rtp_session *session);
/**
 * \brief Sets the \p session RTP port to the provided \p port.
 * \param session The session whose RTP port number is to be set.
 * \warning \p session must not be \c nullptr.
 * \warning As per convention, RTP port must be even.
 * \note As per convention, the RTCP port is one higher than that of RTP.
 */
enum rtp_status rtp_session_set_rtp_port(struct rtp_session *session,
                                         in_port_t port);
in_port_t rtp_session_get_rtp_port(const struct rtp_session *session);
in_port_t rtp_session_get_rtcp_port(const struct rtp_session *session);

#endif // RTP_MOD_RTP_SESSION_H
