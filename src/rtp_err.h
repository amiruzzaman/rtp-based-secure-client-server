#ifndef RTP_MOD_RTP_ERR_H
#define RTP_MOD_RTP_ERR_H

#include <stdint.h>

enum rtp_status : uint8_t {
    STATUS_OK = 0,
    STATUS_BUFF_TOO_SMALL,
    STATUS_NOT_ENOUGH_DATA,
    STATUS_MALLOC_FAILED,
};

const char *rtp_status_summarize(enum rtp_status status);

#endif // !RTP_MOD_RTP_ERR_H
