#include "log.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void) {
    int ret = 0;
    int sock = -1;
    struct sockaddr_in in_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(6969),
        .sin_addr = inet_addr("127.0.0.1"),
    };
    char send_buff[8192];
    *(uint32_t*)(send_buff + 0) = 8192;
    for(size_t i = 4; i < sizeof(send_buff) - 2; ++i) {
        send_buff[i] = 'a';
    }
    send_buff[sizeof(send_buff) - 1] = '\n';
    send_buff[sizeof(send_buff) - 2] = 'b';

    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        REPORT_ERRNO("Failed to create socket");
    }

    if(connect(sock, (struct sockaddr*)&in_addr, sizeof(struct sockaddr_in)) < 0) {
        REPORT_ERRNO("Failed to connect socket");
    }

    send(sock, send_buff, sizeof(send_buff), 0);
defer:
    if (sock > -1) {
        close(sock);
    }
    return ret;
}
