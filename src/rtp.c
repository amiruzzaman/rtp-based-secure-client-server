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
#define BOOL_FLAG(bool_val) (bool_val ? 1 : 0)

static size_t rtp_header_serialize(const struct rtp_header *restrict header,
                                   size_t bufflen,
                                   uint8_t buff[restrict(bufflen + 7) / 8]);

size_t rtp_header_size(const struct rtp_header *header) {
    // 12 octets at least, and then a number of 32-bit CSRCs, and then
    // optionally the extension.
    return 12 * 8 + header->csrc_count * 32 +
           ((header->has_extension) ? header->ext.ext_len : 0);
}

size_t rtp_packet_size(const struct rtp_packet *packet) {
    return rtp_header_size(&packet->header) + packet->payload.data_len;
}

size_t rtp_packet_serialize(const struct rtp_packet *restrict packet,
                            size_t bufflen,
                            uint8_t buff[restrict(bufflen + 7) / 8]) {
    size_t fill_len = rtp_header_serialize(&packet->header, bufflen, buff);
    CHECK_BUFF_LEN(fill_len, bufflen);
    size_t byte_data_len = rtp_util_bits_to_bytes(packet->payload.data_len);

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
                                   uint8_t buff[restrict(bufflen + 7) / 8]) {
    size_t fill_len = 0;
    // first octet
    buff[fill_len] = header->csrc_count + (BOOL_FLAG(header->has_extension) << 4) +
              (BOOL_FLAG(header->has_padding) << 5) + (header->version << 6);
    ++fill_len;

    // second octet
    buff[fill_len] = header->payload_type + (BOOL_FLAG(header->marker) << 7);
    ++fill_len;

    // sequence number
    // TODO

defer:
    return fill_len;
}
