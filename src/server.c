#include "log.h"
#include <arpa/inet.h>
#include <errno.h>
#include <event2/event.h>
#include <event2/thread.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define READ_BUFF_LEN 1024

static void read_callback(evutil_socket_t sock, short what, void *arg);
static int create_ipv4_socket(int* sock, struct addrinfo *info);

int main(void) {
    int ret = 0;
    int sock = -1;
    struct event_base *base = NULL;
    struct event *read_event = NULL;
    char read_buff[READ_BUFF_LEN];
    const struct addrinfo hints = {
        .ai_flags = AI_PASSIVE,
        // make everyone suffer
        .ai_family = PF_UNSPEC,
        .ai_socktype = SOCK_DGRAM,
        .ai_next = NULL,
    };
    struct addrinfo *server_info = NULL;

    // initialize socket
    // if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    //     ret = sock;
    //     REPORT_ERRNO("Socket creation failed");
    // }
    // evutil_make_socket_nonblocking(sock);
    if ((ret = getaddrinfo(NULL, "4200", &hints, &server_info)) != 0) {
        fprintf(stderr, ERROR "getaddrinfo: %s\n", gai_strerror(ret));
        goto defer;
    }
    // get first server address available to bind, and bind
    for (struct addrinfo *pa = server_info; pa != NULL; pa = pa->ai_next) {
        // handle both ipv4 and ipv6
        struct sockaddr_storage *addr = (struct sockaddr_storage *)pa->ai_addr;
        size_t addr_size = sizeof(struct sockaddr_storage);
        char ip_str[INET6_ADDRSTRLEN];
        if((sock = socket(addr->sin_family, SOCK_DGRAM, 0)) < 0) {
            ret = -1;
            printf(INFO "Socket creation failed, retrying\n");
            continue;
        }
        ret = 0;
        if ((ret = bind(sock, (const struct sockaddr *)addr,
                        sizeof(struct sockaddr_in))) != 0) {
            perror(INFO "Bind failed for address ");
            printf("%s port %d\n",
                   inet_ntop(addr->sin_family, (void *)&addr->sin_addr, ip_str,
                             sizeof(struct sockaddr_in)),
                   ntohs(addr->sin_port));
            continue;
        }
        printf(INFO "Bind to %s port %d\n",
               inet_ntop(addr->sin_family, (void *)&addr->sin_addr, ip_str,
                         sizeof(struct sockaddr_in)),
               ntohs(addr->sin_port));
        break;
    }
    if(ret != 0) {
        REPORT_ERRNO("bind");
    }

    // initialize read event
    base = event_base_new();
    if (!base) {
        ret = -1;
        fprintf(stderr, ERROR "Event base creation error\n");
        goto defer;
    }
    read_event = event_new(base, sock, EV_READ, read_callback, read_buff);
    // no timeout
    event_add(read_event, NULL);
    event_base_loop(base, EVLOOP_NONBLOCK);

defer:
    if (server_info) {
        freeaddrinfo(server_info);
    }
    if (read_event) {
        event_free(read_event);
    }
    if (base) {
        event_base_free(base);
    }
    if (sock > -1) {
        close(sock);
    }
    libevent_global_shutdown();
    return ret;
}

static void read_callback(evutil_socket_t sock, short what, void *arg) {
    printf("I READ SOMETHING!!!\n");
}

static int create_ipv4_socket(int* sock, struct addrinfo *info) {
    assert(info->ai_family == AF_INET);
    return 69;
}
