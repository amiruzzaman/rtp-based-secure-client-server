#ifndef RTP_MOD_RTP_ERR_H
#define RTP_MOD_RTP_ERR_H

#include <stdint.h>

enum rtp_status : uint8_t {
    STATUS_OK = 0,
    STATUS_BUFF_TOO_SMALL,
    STATUS_NOT_ENOUGH_DATA,
    STATUS_MALLOC_FAILED,                    // self-explanatory
    STATUS_DESERIALIZE_CSRC_COUNT_TOO_LARGE, // CSRC count larger than 15
    STATUS_DESERIALIZE_VERSION_TOO_LARGE,    // version larger than 2
    STATUS_NO_EXT, // trying to deserialize extension of a packet without
                   // extension.
};

const char *rtp_status_summarize(enum rtp_status status);

#endif // !RTP_MOD_RTP_ERR_H
