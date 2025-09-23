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
    char buffer[8192];
    for(size_t i = 0; i < sizeof(buffer) - 2; ++i) {
        buffer[i] = 'a';
    }
    buffer[sizeof(buffer) - 1] = '\n';
    buffer[sizeof(buffer) - 2] = 'b';

    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        REPORT_ERRNO("Failed to create socket");
    }

    if(connect(sock, (struct sockaddr*)&in_addr, sizeof(struct sockaddr_in)) < 0) {
        REPORT_ERRNO("Failed to connect socket");
    }

    send(sock, buffer, sizeof(buffer), 0);
    char msg[16];

    int msglen = recv(sock, msg, sizeof(msg), 0);
    printf("%.*s\n", msglen, msg);

defer:
    if (sock > -1) {
        close(sock);
    }
    return ret;
}
