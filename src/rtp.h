#ifndef RTP_MOD_RTP_H
#define RTP_MOD_RTP_H

#include "rtp_err.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

struct rtp_ext {
    uint16_t profile_id;
    // length in 4-octet words, including the 4-octet extension header
    uint16_t ext_len;
    uint32_t *ext;
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
    uint32_t csrcs[15];
    /**
     * Might have some extensions.
     */
    struct rtp_ext ext;
};
/**
 * @brief The total size of the header in bytes, including extension if any.
 * @warn Do not pass in a null pointer.
 */
size_t rtp_header_size(const struct rtp_header *header);

struct rtp_payload {
    // data length in bytes
    size_t data_len;
    uint8_t *data;
};
struct rtp_packet {
    struct rtp_header header;
    struct rtp_payload payload;
};
/**
 * @brief Total sie of the packet in bytes
 * @warn Do not pass in a null pointer for packet.
 */
size_t rtp_packet_size(const struct rtp_packet *packet);
/**
 * @brief Serialize the packet into the provided buffer.
 * @warn Do not pass in a null pointer for packet.
 * @note It's possible to pass a null pointer as filled_len, but why?
 *
 * If there's not enough room, STATUS_BUFF_TOO_SMALL is returned.
 */
enum rtp_status rtp_packet_serialize(const struct rtp_packet *restrict packet,
                                     size_t bufflen,
                                     uint8_t buff[restrict bufflen],
                                     size_t *filled_len);

/**
 * @warn Might call malloc; `packet->ext.ext` will need to be freed if that's
 * the case. If `header->has_extension` then it needs to be freed.
 * @warn Do not pass in a null pointer for packet.
 * @warn It is possible to pass in a null pointer for read_len. But, why?
 *
 * If `buff` doesn't contain enough data (e.g. not long enough for all the
 * header components), STATUS_NOT_ENOUGH_DATA is returned.
 */
enum rtp_status rtp_packet_deserialize(struct rtp_packet *restrict packet,
                                       size_t packlen,
                                       uint8_t buff[restrict packlen],
                                       size_t *read_len);

// If you want fine-grained control of where the packet gets deserialized,
// assemble the thing using the 4 functions below

/**
 * @brief Deserialize the packet up until the extension header (if any).
 * @warn Do not pass in a null pointer for header.
 * @warn It is possible to pass in a null pointer for read_len.
 *
 * This is useful to get information about the size of CSRC list and extension
 * header, so that one can prepare a buffer large enough.
 */
enum rtp_status rtp_header_deserialize_pre_ext(
    struct rtp_header *restrict header, size_t bufflen,
    const uint8_t buff[restrict bufflen], size_t *read_len);
/**
 * @brief Deserialize the fixed-size header extension.
 * @param trunc_buff truncates the pre-extension part of the header.
 * @warn Check if there's an extension first, `header->has_extension`.
 * @warn Do not pass in a null pointer for header.
 * @warn It is possible to pass in a null pointer for read_len.
 */
enum rtp_status rtp_header_deserialize_extension_header(
    struct rtp_header *restrict header, size_t bufflen,
    const uint8_t trunc_buff[restrict bufflen], size_t *read_len);
/**
 * @brief Deserialize the variable-size header extension data.
 * @param trunc_buff truncates the pre-extension part of the header, and the
 * 4-octet header extension *header*.
 * @warn Check the size of the header extension data, `header->ext.ext_len`.
 * @warn Do not pass in a null pointer for header.
 * @warn It is possible to pass in a null pointer for read_len.
 */
enum rtp_status rtp_header_deserialize_extension_data(
    struct rtp_header *restrict header, size_t bufflen,
    const uint8_t trunc_buff[restrict bufflen], size_t *read_len);
/**
 * @brief Literally a memcpy from the truncated buffer into the packet payload.
 * @return Either the program crashes because `packet->payload.data_len` is
 * too small, or STATUS_OK.
 *
 * And some more stuff, like setting packet->payload.data_len.
 */
enum rtp_status rtp_packet_deserialize_payload(
    struct rtp_packet *restrict packet, size_t bufflen,
    const uint8_t trunc_buff[restrict bufflen], size_t *read_len);
#endif
