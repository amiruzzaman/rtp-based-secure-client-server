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
    char buffer[1024];

    printf(INFO "Creating socket\n");
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        REPORT_ERRNO("Failed to create socket");
    }

    if (bind(sock, (struct sockaddr *)&in_addr,
                sizeof(struct sockaddr_in)) < 0) {
        REPORT_ERRNO("Failed to bind");
    }

    for (int recvlen = recv(sock, buffer, sizeof(buffer), 0); recvlen > 0;
         recvlen = recv(sock, buffer, sizeof(buffer), 0)) {
        printf("%.*s\n", recvlen, buffer);
    }
    send(sock, "Bye", sizeof("Bye"), 0);

defer:
    if (sock > -1) {
        close(sock);
    }
    return ret;
}
