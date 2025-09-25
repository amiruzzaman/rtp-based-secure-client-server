#include "rtp.h"
#include "rtp_util.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define CHECK_BUFF_LEN(curr_idx, max_siz)                                      \
    do {                                                                       \
        if (curr_idx >= max_siz) {                                             \
            goto defer;                                                        \
        }                                                                      \
    } while (0)
#define BOOL_FLAG(bool_val) ((bool_val) ? 1 : 0)

#define REINTERPRET_CAST(type, lvalue) \
    ((type*)(lvalue))

static size_t rtp_header_serialize(const struct rtp_header *restrict header,
                                   size_t bufflen,
                                   uint8_t buff[restrict bufflen]);

size_t rtp_header_size(const struct rtp_header *header) {
    // 12 octets at least, and then a number of 32-bit CSRCs, and then
    // optionally the extension.
    return 12 + header->csrc_count * 4 +
           ((header->has_extension) ? header->ext.ext_len * 4 : 0);
}

size_t rtp_packet_size(const struct rtp_packet *packet) {
    return rtp_header_size(&packet->header) + packet->payload.data_len;
}

size_t rtp_packet_serialize(const struct rtp_packet *restrict packet,
                            size_t bufflen,
                            uint8_t buff[restrict bufflen]) {
    CHECK_BUFF_LEN(bufflen, rtp_packet_size(packet));

    size_t fill_len = rtp_header_serialize(&packet->header, bufflen, buff);
    size_t byte_data_len = RTP_UTIL_BITS_TO_BYTES(packet->payload.data_len);

    size_t memcpy_len = (byte_data_len < fill_len - bufflen)
                            ? byte_data_len
                            : fill_len - bufflen;
    memcpy(buff + fill_len, packet->payload.data, memcpy_len);
    fill_len += memcpy_len;

defer:
    return fill_len;
}

static size_t rtp_header_serialize(const struct rtp_header *restrict header,
                                   size_t bufflen,
                                   uint8_t buff[restrict bufflen]) {
    size_t fill_len = 0;
    // 1st octet
    buff[fill_len] = header->csrc_count + (BOOL_FLAG(header->has_extension) << 4) +
              (BOOL_FLAG(header->has_padding) << 5) + (header->version << 6);
    ++fill_len;

    // 2nd octet
    buff[fill_len] = header->payload_type + (BOOL_FLAG(header->marker) << 7);
    ++fill_len;

    // 3th and 4th octets, sequence number
    *REINTERPRET_CAST(uint16_t, buff + fill_len) = header->seq_num;
    fill_len += sizeof(uint16_t);

    // 5th to 8th octets, timestamp
    *REINTERPRET_CAST(uint32_t, buff + fill_len) = header->timestamp;
    fill_len += sizeof(uint32_t);

    // 9th to 12th octets, SSRC
    *REINTERPRET_CAST(uint32_t, buff + fill_len) = header->ssrc;
    fill_len += sizeof(uint32_t);

    // some amount of 4-octets more, CSRC list
    for(uint8_t csrc = 0; csrc < header->csrc_count;
            fill_len += sizeof(uint32_t), ++csrc) {
        *REINTERPRET_CAST(uint32_t, buff + fill_len) = header->csrcs[csrc];
    }

    // if there's extension, fill that in also
    if(header->has_extension) {
        // 2 octets profile id, 2 octets extention length
        *REINTERPRET_CAST(uint16_t, buff + fill_len) = header->ext.profile_id;
        fill_len += sizeof(uint16_t);
        *REINTERPRET_CAST(uint16_t, buff + fill_len) = header->ext.ext_len;
        fill_len += sizeof(uint16_t);
        // fill in the extension
        for(size_t ext_idx = 0; ext_idx < header->ext.ext_len - 1;
                ++ext_idx, fill_len += 4) {
            *REINTERPRET_CAST(uint32_t, buff + fill_len) = header->ext.ext[ext_idx];
        }
    }

    return fill_len;
}

// TODO: deserialization.

size_t rtp_packet_deserialize(struct rtp_packet *restrict packet,
                              size_t bufflen,
                              uint8_t buff[restrict bufflen]) {
    size_t pre_ext_header_size = rtp_header_deserialize_pre_ext(&packet->header, bufflen, buff);
    // TODO
}
