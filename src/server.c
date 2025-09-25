#include "log.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
// testing if the thing links
#include <event2/event.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    int ret = 0;
    int sock = -1;
    struct sockaddr_in client_addr = {0};
    uint8_t recv_buff[1024];
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(4200),
        .sin_addr = INADDR_ANY,
    };
    socklen_t client_addr_len = sizeof(client_addr);

    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        REPORT_ERRNO("Socket creation failed");
    }

    if(bind(sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in)) < 0) {
        REPORT_ERRNO("Socket binding failed");
    }

    ssize_t recv_len = recvfrom(sock, recv_buff, sizeof(recv_buff), 0,
                        (struct sockaddr*)&client_addr, &client_addr_len);
    printf("Received: %.*s\n", (int)recv_len, recv_buff);
    printf("Sending back BYE message\n");
    sendto(sock, "BYE", sizeof("BYE"), 0, (const struct sockaddr*)&client_addr, client_addr_len);

defer:
    if(sock > -1) {
        close(sock);
    }
    return ret;
}
