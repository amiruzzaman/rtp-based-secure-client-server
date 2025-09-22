#define REPORT_ERRNO(msg)                                                      \
    do {                                                                       \
        perror(msg);                                                           \
        ret = errno;                                                           \
        goto defer;                                                            \
    } while (0)

int main(void) {
    int ret = 0;
    int in_sock = -1;
defer:
    return ret;
}
