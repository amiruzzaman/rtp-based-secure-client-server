#include "log.h"
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <event2/event.h>
#include <event2/thread.h>
#include <event2/util.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define READ_BUFF_LEN 1400

struct read_event_arguments {
    struct event *self;
    struct sockaddr *recv_addr;
    socklen_t recv_addr_len;
    char read_buff[READ_BUFF_LEN];
};
static void read_callback(evutil_socket_t sock, short what, void *arg);

int main(void) {
    int ret = 0;
    evutil_socket_t sock = -1;
    struct event_base *base = NULL;
    struct event *read_event = NULL;
    struct sockaddr_in6 recv_addr = {
        .sin6_family = AF_INET6,
    };
    struct read_event_arguments read_args = {
        .recv_addr = (struct sockaddr *)&recv_addr,
    };
    const struct addrinfo hints = {
        .ai_flags = AI_PASSIVE,
        .ai_family = AF_INET6,
        .ai_socktype = SOCK_DGRAM,
    };
    struct addrinfo *server_info = NULL;

    if ((ret = getaddrinfo(NULL, "4200", &hints, &server_info)) != 0) {
        fprintf(stderr, ERROR "getaddrinfo: %s\n", gai_strerror(ret));
        goto defer;
    }
    // get first server address available to bind, and bind
    for (struct addrinfo *pa = server_info; pa != NULL; pa = pa->ai_next) {
        // handle both ipv4 and ipv6
        // TODO: we explicitly request only IPv6, so no need for a lot of these
        // complicated logics.
        struct sockaddr_storage *addr = (struct sockaddr_storage *)pa->ai_addr;
        socklen_t addr_size = sizeof(struct sockaddr_in6);
        if ((sock = socket(addr->ss_family, SOCK_DGRAM, 0)) < 0) {
            perror(INFO "Retrying socket creation");
            continue;
        }
        // IPv4-mapped IPv6
        if (addr->ss_family == AF_INET6) {
            int nope = 0;
            if (setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &nope,
                           sizeof(int)) != 0) {
                perror(INFO "Set socket option error, retrying");
                continue;
            }
        }
        // reuse address
        {
            int yep = 1;
            if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yep, sizeof(int)) !=
                0) {
                perror(INFO "Set reuse address error, retrying");
                continue;
            }
        }
        evutil_make_socket_nonblocking(sock);
        if ((ret = bind(sock, (struct sockaddr *)addr, addr_size)) != 0) {
            perror(INFO "Retrying socket creation");
            close(sock);
            continue;
        }
        char addr_str[INET6_ADDRSTRLEN];
        void *in_addr = &(((struct sockaddr_in6 *)addr)->sin6_addr);
        inet_ntop(AF_INET6, in_addr, addr_str, INET6_ADDRSTRLEN);
        printf(INFO "Bind socket to address %s\n", addr_str);
        break;
    }

    // if error occurs
    if (sock < 0) {
        REPORT_ERRNO("socket");
    }
    if (ret != 0) {
        REPORT_ERRNO("bind");
    }

    // initialize read event
    base = event_base_new();
    if (!base) {
        ret = -1;
        fprintf(stderr, ERROR "Event base creation error\n");
        goto defer;
    }
    // TODO: install a signal handler or some `atexit` functions to clean up
    // resources.
    read_event =
        event_new(base, sock, EV_READ | EV_PERSIST, read_callback, &read_args);

    read_args.self = read_event;
    // no timeout
    event_add(read_event, NULL);
    // TODO: flip back to event_base_loop when we have to do more than just
    // receiving packages.
    event_base_dispatch(base);
    // event_base_loop(base, EVLOOP_NONBLOCK);

defer:
    printf(INFO "Shutdown server\n");
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
    struct read_event_arguments *read_args = (struct read_event_arguments *)arg;
    assert(read_args != NULL);
    ssize_t msglen =
        recvfrom(sock, read_args->read_buff, sizeof(read_args->read_buff), 0,
                 read_args->recv_addr, &(read_args->recv_addr_len));

    char addr_str[INET6_ADDRSTRLEN];
    void *in_addr =
        read_args->recv_addr->sa_family == AF_INET
            ? &((struct sockaddr_in *)read_args->recv_addr)->sin_addr
            : &((struct sockaddr_in6 *)read_args->recv_addr)->sin6_addr;
    inet_ntop(read_args->recv_addr->sa_family, in_addr, addr_str,
              INET6_ADDRSTRLEN);
    if (read_args->read_buff[msglen - 1] == '\n') {
        --msglen;
    }
    printf(INFO "Received \"%.*s\" from %s\n", (int)msglen,
           read_args->read_buff, addr_str);
}
