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

int main(void) {
    uint32_t clk = (uint32_t)clock();
    uint32_t ssrc_rand = arc4random_uniform(UINT32_MAX / 4);
    struct rtp_header minimal_test_header = {
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
        .ssrc = ssrc_rand,
        .csrcs = {},
        .ext = {},
    };
    assert(rtp_header_size(&minimal_test_header) == RTP_HEADER_MIN_SIZE);

    uint8_t header_buff[RTP_HEADER_MIN_SIZE];
    size_t fill_len = 0;
    enum rtp_status rc = rtp_header_serialize(
        &minimal_test_header, sizeof(header_buff), header_buff, &fill_len);
    assert(rc == STATUS_OK);
    assert(fill_len == RTP_HEADER_MIN_SIZE);

    struct rtp_header minimal_deserialize_header = {};
    size_t read_len = 0;
    // TODO: this function down here
    rc = rtp_header_deserialize_pre_ext(&minimal_deserialize_header, fill_len,
                                        header_buff, &read_len);
    assert(rc == STATUS_OK);
    assert(read_len == RTP_HEADER_MIN_SIZE);

    // printf("deserialize.version = %d\n", minimal_deserialize_header.version);
    // first octet
    assert(minimal_deserialize_header.version == 2);
    assert(!minimal_deserialize_header.has_padding);
    assert(!minimal_deserialize_header.has_extension);
    assert(minimal_deserialize_header.csrc_count == 0);

    // second octet
    assert(!minimal_deserialize_header.marker);
    assert(minimal_deserialize_header.payload_type == 69);

    // the other information
    assert(minimal_deserialize_header.seq_num == 420);
    assert(minimal_deserialize_header.timestamp == clk);
    assert(minimal_deserialize_header.ssrc == ssrc_rand);

    return 0;
}
