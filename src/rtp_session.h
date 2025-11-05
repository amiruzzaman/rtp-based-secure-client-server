#ifndef RTP_MOD_RTP_SESSION_H
#define RTP_MOD_RTP_SESSION_H

#include <stddef.h>
#include <stdint.h>

// TODO: the sending RTP session and receiving RTP session should be different.

/**
 * Each type of stream has its own RTP session.
 */
struct rtp_session {
    // actual number of packets received.
    uint64_t n_pack_received;
    // since sequence number is only 16 bits long, there's real chance it will
    // wrap around. When it does, increment this number.
    // So, the sequence number we use to compare packets' sequence numbers
    // should be (n_shifted_seq_cycles << 16) + sequence number
    uint32_t n_shifted_seq_cycles;
    // this synchronization source.
    uint32_t ssrc;
    // timestamp of the last package sent/received.
    uint32_t timestamp;
    // default timestamp increment.
    uint32_t timestamp_inc;
    // max sequence number sent/received.
    uint16_t max_seq_num;
    uint8_t payload_type;
};

#endif
