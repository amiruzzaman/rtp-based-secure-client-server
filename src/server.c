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

int main(void) {
    int ret = 0;
    int sock = -1;
    struct sockaddr_in in_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(6969),
        .sin_addr = {inet_addr("127.0.0.1")},
    };
    uint8_t *recv_buff = NULL;

    printf(INFO "Creating socket\n");
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        REPORT_ERRNO("Failed to create socket");
    }

    if (bind(sock, (struct sockaddr *)&in_addr,
                sizeof(struct sockaddr_in)) < 0) {
        REPORT_ERRNO("Failed to bind");
    }
    
    uint8_t header[sizeof(uint32_t)/sizeof(uint8_t)];
    ssize_t recv_size = recv(sock, header, sizeof(header), MSG_PEEK);
    if(recv_size < (ssize_t)sizeof(header)) {
        REPORT_ERRNO("Data peeking failed: header size unexpected");
    }

    // If we just call `recv` without checking packet size, our packet will
    // get truncated if it's too large.
    uint32_t packet_size = *(uint32_t*)(header + 0);
    printf("Expected packet of size %u\n", packet_size);

    recv_buff = (uint8_t*)malloc(packet_size);
    if(recv_buff == NULL) {
        REPORT_ERRNO("Malloc failed");
    }
    recv_size = recv(sock, recv_buff, packet_size, 0);
    if(recv_size != packet_size) {
        REPORT_ERRNO("Truncated or overfilled packet");
    }

    printf("Received:\n%.*s\n",
            (int)((size_t)recv_size - sizeof(header)),
            (recv_buff + sizeof(header)));

defer:
    if (sock > -1) {
        close(sock);
    }
    if(recv_buff != NULL) {
        free(recv_buff);
    }
    return ret;
}
