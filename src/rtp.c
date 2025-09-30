#include "rtp.h"
#include "rtp_err.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

// requires a defer tag and a `enum rtp_status ret;`
#define CHECK_BUFF_LEN(curr_idx, max_siz)                                      \
    do {                                                                       \
        if (curr_idx >= max_siz) {                                             \
            ret = STATUS_BUFF_TOO_SMALL;                                       \
            goto defer;                                                        \
        }                                                                      \
    } while (0)
#define BOOL_FLAG(bool_val) ((bool_val) ? 1 : 0)

#define REINTERPRET_CAST(type, lvalue) ((type *)(lvalue))

size_t rtp_header_size(const struct rtp_header *header) {
    assert(header != NULL);
    // 12 octets at least, and then a number of 32-bit CSRCs, and then
    // optionally the extension.
    return 12 + header->csrc_count * 4 +
           ((header->has_extension) ? header->ext.ext_len * 4 : 0);
}

size_t rtp_packet_size(const struct rtp_packet *packet) {
    assert(packet != NULL);
    return rtp_header_size(&packet->header) + packet->payload.data_len;
}

enum rtp_status rtp_packet_serialize(const struct rtp_packet *restrict packet,
                                     size_t bufflen,
                                     uint8_t buff[restrict bufflen],
                                     size_t *filled_len) {
    assert(packet != NULL);

    enum rtp_status ret = STATUS_OK;
    const size_t packet_size = rtp_packet_size(packet);
    size_t fill_len = 0;

    if (packet_size > bufflen) {
        ret = STATUS_BUFF_TOO_SMALL;
        goto defer;
    }

    ret = rtp_header_serialize(&packet->header, bufflen, buff, &fill_len);
    if (ret != STATUS_OK) {
        goto defer;
    }
    // since we did check that the buffer is large enough already
    assert(bufflen - fill_len >= packet->payload.data_len);
    memcpy(buff + fill_len, packet->payload.data, packet->payload.data_len);
    fill_len += packet->payload.data_len;

defer:
    if (filled_len != NULL) {
        *filled_len = fill_len;
    }
    return ret;
}

enum rtp_status rtp_header_serialize(const struct rtp_header *restrict header,
                                     size_t bufflen,
                                     uint8_t buff[restrict bufflen],
                                     size_t *filled_len) {
    assert(header != NULL);

    enum rtp_status ret = STATUS_OK;
    size_t fill_len = 0;

    if (bufflen < rtp_header_size(header)) {
        ret = STATUS_BUFF_TOO_SMALL;
        goto defer;
    }

    // 1st octet
    buff[fill_len] =
        (uint8_t)(header->csrc_count + (BOOL_FLAG(header->has_extension) << 4) +
                  (BOOL_FLAG(header->has_padding) << 5) +
                  (header->version << 6));
    ++fill_len;

    // 2nd octet
    buff[fill_len] =
        (uint8_t)(header->payload_type + (BOOL_FLAG(header->marker) << 7));
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
    for (uint8_t csrc = 0; csrc < header->csrc_count;
         fill_len += sizeof(uint32_t), ++csrc) {
        *REINTERPRET_CAST(uint32_t, buff + fill_len) = header->csrcs[csrc];
    }

    // if there's extension, fill that in also
    if (header->has_extension) {
        // 2 octets profile id, 2 octets extention length
        *REINTERPRET_CAST(uint16_t, buff + fill_len) = header->ext.profile_id;
        fill_len += sizeof(uint16_t);
        *REINTERPRET_CAST(uint16_t, buff + fill_len) = header->ext.ext_len;
        fill_len += sizeof(uint16_t);
        // fill in the extension
        for (size_t ext_idx = 0; ext_idx < header->ext.ext_len - 1;
             ++ext_idx, fill_len += 4) {
            *REINTERPRET_CAST(uint32_t, buff + fill_len) =
                header->ext.ext[ext_idx];
        }
    }

defer:
    if (filled_len != NULL) {
        *filled_len = fill_len;
    }
    return ret;
}

// TODO: deserialization.

enum rtp_status rtp_packet_deserialize(struct rtp_packet *restrict packet,
                                       size_t packlen,
                                       uint8_t buff[restrict packlen],
                                       size_t *read_len) {
    assert(packet != NULL);

    enum rtp_status ret = STATUS_OK;
    size_t read_size = 0;
    size_t buff_idx = 0;
    // header-filling
    ret = rtp_header_deserialize_pre_ext(&packet->header, packlen, buff,
                                         &buff_idx);
    if (ret != STATUS_OK) {
        goto defer;
    }
    read_size += buff_idx;

    ret = rtp_header_deserialize_extension_header(
        &packet->header, packlen - read_size, buff + read_size, &buff_idx);
    if (ret != STATUS_OK) {
        goto defer;
    }
    read_size += buff_idx;

    ret = rtp_header_deserialize_extension_data(
        &packet->header, packlen - read_size, buff + read_size, &buff_idx);
    if (ret != STATUS_OK) {
        goto defer;
    }
    read_size += buff_idx;
    packet->payload.data = (uint8_t *)malloc(packlen - read_size);
    if (packet->payload.data == NULL) {
        ret = STATUS_MALLOC_FAILED;
        goto defer;
    }
    rtp_packet_deserialize_payload(packet, packlen - read_size,
                                   buff + read_size, &buff_idx);
    read_size += buff_idx;

defer:
    if (read_len != NULL) {
        *read_len = buff_idx;
    }
    return ret;
}

enum rtp_status rtp_packet_deserialize_payload(
    struct rtp_packet *restrict packet, size_t bufflen,
    const uint8_t trunc_buff[restrict bufflen], size_t *read_len) {
    assert(packet != NULL);

    enum rtp_status ret = STATUS_OK;
    memcpy(packet->payload.data, trunc_buff, bufflen);
    packet->payload.data_len = bufflen;
    if (read_len != NULL) {
        *read_len = bufflen;
    }
    return ret;
}

enum rtp_status rtp_header_deserialize_pre_ext(
    struct rtp_header *restrict header, size_t bufflen,
    const uint8_t buff[restrict bufflen], size_t *read_len) {
    assert(header != NULL);
    assert(bufflen >= RTP_HEADER_MIN_SIZE);

    enum rtp_status ret = STATUS_OK;
    size_t curr_read_len = 0;
    // read rtp_header_serialize
    {
        uint8_t first_octet = buff[0];
        // get the 4 bits
        header->csrc_count = (first_octet & (UINT8_MAX >> 4));
        if(header->csrc_count > 15) {
            ret = STATUS_DESERIALIZE_CSRC_COUNT_TOO_LARGE;
            goto defer;
        }
        first_octet >>= 4;
        header->has_extension = (first_octet & 0b1);
        first_octet >>= 1;
        header->has_padding = (first_octet & 0b1);
        first_octet >>= 1;
        // at this point this only contains the last 2 bits
        assert((first_octet & 0b11111100) == 0);
        header->version = first_octet;
        if(header->version > 2) {
            ret = STATUS_DESERIALIZE_VERSION_TOO_LARGE;
            goto defer;
        }
        ++curr_read_len;
    }

    {
        uint8_t second_octet = buff[1];
        header->payload_type = (second_octet & (UINT8_MAX >> 1));
        second_octet >>= 7;
        // at this point this only contains the last bit
        assert((second_octet & UINT8_MAX << 1) == 0);
        header->marker = second_octet;
        ++curr_read_len;
    }

    header->seq_num = *(uint16_t *)(buff + 2);
    curr_read_len += 2;
    header->timestamp = *(uint32_t *)(buff + 4);
    curr_read_len += 4;
    header->ssrc = *(uint32_t *)(buff + 8);
    curr_read_len += 4;

    for (uint32_t csrc_idx = 0; csrc_idx < header->csrc_count;
         ++csrc_idx, curr_read_len += 4) {
        header->csrcs[csrc_idx] = *(uint32_t *)(buff + curr_read_len);
    }
defer:
    if (read_len != NULL) {
        *read_len = curr_read_len;
    }
    return ret;
}

enum rtp_status rtp_header_deserialize_extension_header(
    struct rtp_header *restrict header, size_t bufflen,
    const uint8_t trunc_buff[restrict bufflen], size_t *read_len) {
    // TODO
    return STATUS_OK;
}

enum rtp_status rtp_header_deserialize_extension_data(
    struct rtp_header *restrict header, size_t bufflen,
    const uint8_t trunc_buff[restrict bufflen], size_t *read_len) {
    // TODO
    return STATUS_OK;
}
