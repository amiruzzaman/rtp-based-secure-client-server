#ifndef RTP_MOD_RTP_SESSION_H
#define RTP_MOD_RTP_SESSION_H

#include <stdint.h>

/**
 * Each type of stream has its own RTP session.
 */
struct rtp_session {
    // actual number of packets received.
    uint64_t n_pack_received;
    // since sequence number is only 16 bits long, there's real chance it will
    // wrap around. When it does, increment this number.
    uint64_t n_shifted_seq_cycles;
    // this synchronization source.
    uint32_t ssrc; 
    uint32_t timestamp;
    // default timestamp increment.
    uint32_t timestamp_inc;
    // might be wrapped.
    uint16_t max_seq_num;
    uint16_t base_seq_num;
    uint8_t payload_type;
};

/**
 * Initializes the following in the RTP session:
 * - Pick a random SSRC.
 * - Pick a random timestamp.
 * - Record the wall-clock.
 * - Assign the payload type.
 */
struct rtp_session rtp_session_init(uint8_t payload_type);

#endif
