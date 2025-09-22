#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#define REPORT_ERRNO(msg)                                                      \
    do {                                                                       \
        perror(msg);                                                           \
        ret = errno;                                                           \
        goto defer;                                                            \
    } while (0)

int main(void) {
    int ret = 0;
    int in_sock = -1;
    int comm_sock = -1;
    struct sockaddr_in in_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(6969),
        .sin_addr = inet_addr("127.0.0.1"),
    };
    char buffer[1024];
    printf("Creating socket\n");
    in_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (in_sock == -1) {
        REPORT_ERRNO("Failed to create socket");
    }
    int optval = 1;
    if (setsockopt(in_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval,
                   sizeof(optval)) == -1) {
        REPORT_ERRNO("Failed to set socket options");
    }
    printf("Binding socket to 127.0.0.1:6969\n");
    if (bind(in_sock, (struct sockaddr *)&in_addr,
             sizeof(struct sockaddr_in)) == -1) {
        REPORT_ERRNO("Failed to bind socket");
    }
    printf("Bind success\nListening to connections\n");
    if (listen(in_sock, 1) == -1) {
        REPORT_ERRNO("Failed to listen to socket");
    }
    printf("Accepting incoming connections\n");
    comm_sock = accept(in_sock, nullptr, 0);
    if (comm_sock == -1) {
        REPORT_ERRNO("Failed to accept incoming requests");
    }

    struct sockaddr_in peer_addr = {
        .sin_family = AF_INET,
        // zero-init the other parts
    };
    socklen_t peer_addrlen = sizeof(struct sockaddr_in);
    if(!getpeername(in_sock, (struct sockaddr*)&peer_addr, &peer_addrlen)) {
        REPORT_ERRNO("Failed to get peer name");
    }
    printf("Connected to peer %s\n", inet_ntoa(peer_addr.sin_addr));

    int recv_siz = recv(comm_sock, buffer, sizeof(buffer) - 1, 0);
    buffer[recv_siz] = '\0';
    printf("Received:\n%s\n", buffer);

defer:
    if (in_sock > -1) {
        close(in_sock);
    }
    return ret;
}
