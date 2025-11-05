#ifndef RTP_MOD_LOG_H
#define RTP_MOD_LOG_H

#define INFO "\x1b[1m[\x1b[36mINFO\x1b[0m\x1b[1m]\x1b[0m "
#define ERROR "\x1b[1m[\x1b[31mERROR\x1b[0m\x1b[1m]\x1b[0m "

#define REPORT_ERRNO(msg)                                                      \
    do {                                                                       \
        perror(ERROR msg);                                                     \
        ret = errno;                                                           \
        goto defer;                                                            \
    } while (0)

#endif
