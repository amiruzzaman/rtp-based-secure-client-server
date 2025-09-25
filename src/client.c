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
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(4200),
        .sin_addr = INADDR_ANY,
    };
    socklen_t server_addr_len = sizeof(server_addr);
    uint8_t recv_buff[1024];

    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        REPORT_ERRNO("Socket creation failed");
    }

    ssize_t sendto_len = sendto(sock, "HELLO", sizeof("HELLO"), 0,
                                (const struct sockaddr*)&server_addr,
                                sizeof(struct sockaddr_in));
    if(sendto_len < 0) {
        REPORT_ERRNO("Sending message failed");
    }

    ssize_t recv_len = recvfrom(sock, recv_buff, sizeof(recv_buff), 0,
            (struct sockaddr*)&server_addr, &server_addr_len);
    printf("Received: %.*s\n", (int)recv_len, recv_buff);

defer:
    if(sock > -1) {
        close(sock);
    }
    return ret;
}
