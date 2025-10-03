#include "rtp.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// TODO: get some actual error-reporting facility
// Say, a formatter,
// an actual ASSERT_EQ or of sort.

void test_minimal_header(void) {
    uint32_t clk = (uint32_t)clock();
    uint32_t ssrc_rand = arc4random_uniform(UINT32_MAX / 4);
    struct rtp_header test_header = {
        .version = 2,
        .has_padding = false,
        .has_extension = false,
        .csrc_count = 0,
        .marker = false,
        // just some random numbers
        .payload_type = 69,
        .seq_num = 420,
        // surely this is safe
        .timestamp = clk,
        // actual random hehe
        .ssrc = ssrc_rand,
        .csrcs = {},
        .ext = {},
    };
    assert(rtp_header_size(&test_header) == RTP_HEADER_MIN_SIZE);

    uint8_t header_buff[RTP_HEADER_MIN_SIZE];
    size_t fill_len = 0;
    enum rtp_status rc = rtp_header_serialize(&test_header, sizeof(header_buff),
                                              header_buff, &fill_len);
    assert(rc == STATUS_OK);
    assert(fill_len == RTP_HEADER_MIN_SIZE);

    struct rtp_header deserialize_header = {};
    size_t read_len = 0;
    // TODO: this function down here
    rc = rtp_header_deserialize_pre_ext(&deserialize_header, fill_len,
                                        header_buff, &read_len);
    assert(rc == STATUS_OK);
    assert(read_len == RTP_HEADER_MIN_SIZE);

    // printf("deserialize.version = %d\n", deserialize_header.version);
    // first octet
    assert(deserialize_header.version == 2);
    assert(!deserialize_header.has_padding);
    assert(!deserialize_header.has_extension);
    assert(deserialize_header.csrc_count == 0);

    // second octet
    assert(!deserialize_header.marker);
    assert(deserialize_header.payload_type == 69);

    // the other information
    assert(deserialize_header.seq_num == 420);
    assert(deserialize_header.timestamp == clk);
    assert(deserialize_header.ssrc == ssrc_rand);
}

void test_header_with_csrc(void) {
    uint32_t clk = (uint32_t)clock();
    uint32_t ssrc_rand = arc4random_uniform(UINT32_MAX / 4);
    struct rtp_header test_header = {
        .version = 2,
        .has_padding = false,
        .has_extension = false,
        // we have some CSRCs now
        .csrc_count = 4,
        .marker = false,
        // just some random numbers
        .payload_type = 69,
        .seq_num = 420,
        // surely this is safe
        .timestamp = clk,
        // actual random hehe
        .ssrc = ssrc_rand,
        .csrcs = {12, 23, 34, 45},
        .ext = {},
    };
    assert(rtp_header_size(&test_header) == RTP_HEADER_MIN_SIZE + 4 * 4);
    uint8_t header_buff[RTP_HEADER_MIN_SIZE + 4 * 4];
    size_t fill_len = 0;
    enum rtp_status rc = rtp_header_serialize(&test_header, sizeof(header_buff),
                                              header_buff, &fill_len);
    assert(rc == STATUS_OK);
    assert(fill_len == sizeof(header_buff));

    struct rtp_header test_deserialize_header = {};
    size_t read_len = 0;
    rc = rtp_header_deserialize_pre_ext(&test_deserialize_header, fill_len,
                                        header_buff, &read_len);
    assert(rc == STATUS_OK);
    assert(read_len == fill_len);

    assert(test_deserialize_header.version == 2);
    assert(!test_deserialize_header.has_padding);
    assert(!test_deserialize_header.has_extension);
    assert(test_deserialize_header.csrc_count == 4);
    assert(!test_deserialize_header.marker);
    assert(test_deserialize_header.payload_type == 69);
    assert(test_deserialize_header.seq_num == 420);
    assert(test_deserialize_header.timestamp == clk);
    assert(test_deserialize_header.ssrc == ssrc_rand);
    assert(test_deserialize_header.csrcs[0] == 12);
    assert(test_deserialize_header.csrcs[1] == 23);
    assert(test_deserialize_header.csrcs[2] == 34);
    assert(test_deserialize_header.csrcs[3] == 45);
}

void test_header_with_ext(void) {
    uint32_t clk = (uint32_t)clock();
    uint32_t ssrc_rand = arc4random_uniform(UINT32_MAX / 4);
    struct rtp_header test_header = {
        .version = 2,
        .has_padding = false,
        .has_extension = true,
        // we have some CSRCs now
        .csrc_count = 0,
        .marker = false,
        // just some random numbers
        .payload_type = 69,
        .seq_num = 420,
        // surely this is safe
        .timestamp = clk,
        // actual random hehe
        .ssrc = ssrc_rand,
        .csrcs = {},
        .ext =
            {
                .profile_id = 69,
                .ext_len = 4,
                // compound literals have addresses.
                .ext = (uint32_t[]){1, 2, 3, 4},
            },
    };
    assert(rtp_header_size(&test_header) == RTP_HEADER_MIN_SIZE + 4 * 4);
    uint8_t buff[RTP_HEADER_MIN_SIZE + 4 * 4];
    size_t fill_len = 0;
    enum rtp_status rc = rtp_header_serialize(
        &test_header, RTP_HEADER_MIN_SIZE + 4 * 4, buff, &fill_len);
    assert(rc == STATUS_OK);
    assert(fill_len == RTP_HEADER_MIN_SIZE + 4 * 4);

    struct rtp_header deserialize_header = {};
    size_t read_len = 0;
    // this function does not read in the extension yet
    rc = rtp_header_deserialize_pre_ext(
        &deserialize_header, RTP_HEADER_MIN_SIZE + 4 * 4, buff, &read_len);
    assert(rc == STATUS_OK);
    assert(read_len == RTP_HEADER_MIN_SIZE);

    assert(deserialize_header.has_extension);
    uint8_t *trunc_buff = buff + read_len;
    rc = rtp_header_deserialize_extension_header(
        &deserialize_header, fill_len - read_len, trunc_buff, &read_len);
    assert(rc == STATUS_OK);
    assert(read_len == RTP_HEADER_EXT_HEADER_SIZE);

    assert(deserialize_header.ext.profile_id == 69);
    assert(deserialize_header.ext.ext_len == 4);
    // TODO: deserialize the extension data
}

int main(void) {
    test_minimal_header();
    test_header_with_csrc();
    test_header_with_ext();
    return 0;
}
