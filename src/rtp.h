#ifndef RTP_MOD_RTP_H
#define RTP_MOD_RTP_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

struct rtp_ext {
    uint16_t profile_id;
    // length in bits
    uint16_t ext_len;
    uint8_t *ext;
};
// TODO: since we have to serialize the packet before sending anyways, why not
// make the header more readable? Y'know, like separating the first octet into
// separate fields.

/**
 * @brief Host byte order RTP header.
 *
 * From [RFC3550](https://www.rfc-editor.org/rfc/rfc3550#section-5.1)
 *
 * Also, feel free to access the content of the header. Please don't modify it
 * though.
 */
struct rtp_header {
    /** @brief Bit 0 to 7
     *
     * 2-bit V, 1-bit P, 1-bit X, 4-bit CC
     */

    // bit 0, 2-bit
    uint8_t version;
    // bit 2, 1-bit
    bool has_padding;
    // bit 3, 1-bit
    bool has_extension;
    // bit 4, 4-bit
    uint8_t csrc_count;

    /**
     * @brief Bit 8 to 15
     *
     * 1-bit M, 7-bit PT
     */

    // bit 8, 1-bit
    bool marker;
    // bit 9, 7-bit
    uint8_t payload_type;

    /**
     * @brief Bit 16 to 31
     *
     * 16-bit sequence number
     */
    uint16_t seq_num;
    /**
     * @brief Bit 32 to 63
     *
     * 32-bit timestamp
     */
    uint32_t timestamp;
    /**
     * @brief Bit 64 to 95
     *
     * 32-bit SSRC
     */
    uint32_t ssrc;
    /**
     * @brief from Bit 96
     *
     * 0 to 15 CSRC sources. The number of CSRCs is CC (in our case,
     * csrc_count).
     */
    uint32_t *csrcs;
    /**
     * Might have some extensions.
     */
    struct rtp_ext ext;
};
/**
 * @brief The total size of the header in bits, including extension if any.
 * @warn Do not pass in a null pointer.
 */
size_t rtp_header_size(const struct rtp_header *header);

struct rtp_payload {
    // data length in bits
    size_t data_len;
    uint8_t *data;
};
struct rtp_packet {
    struct rtp_header header;
    struct rtp_payload payload;
};
/**
 * @brief Total sie of the packet in bits
 */
size_t rtp_packet_size(const struct rtp_packet *packet);
/**
 * @brief Serialize the packet into the provided buffer.
 * @warn Do not pass in a null pointer as packet.
 * @return The size filled in.
 *
 * bufflen can be obtained from calling `rtp_packet_size`.
 * If the buffer size is shorter than `rtp_packet_size(packet)`, the packet is
 * truncated.
 * The signature requires buff\[restrict(bufflen + 7)/8\] in order to round-up
 * the division (that is, bufflen is size in bits, but buff is indexed in
 * bytes)
 */
size_t rtp_packet_serialize(const struct rtp_packet *restrict packet,
                             size_t bufflen,
                             uint8_t buff[restrict(bufflen + 7) / 8]);

#endif
