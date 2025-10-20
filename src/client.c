#include "log.h"
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <event2/event.h>
#include <event2/util.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define WRITE_BUFF_LEN 1024

struct write_event_arguments {
    struct event *self;
    struct sockaddr *send_addr;
    socklen_t send_addr_len;
    char write_buff[WRITE_BUFF_LEN];
};

static void write_callback(evutil_socket_t sock, short what, void *arg);
int main(void) {
    int ret = 0;
    int sock = -1;
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_DGRAM,
    };
    struct event_base *base = NULL;
    struct event *write_event = NULL;
    struct write_event_arguments write_args = {};
    struct addrinfo *server_info = NULL;
    if ((ret = getaddrinfo("localhost", "4200", &hints, &server_info)) != 0) {
        fprintf(stderr, ERROR "getaddrinfo: %s\n", gai_strerror(ret));
        goto defer;
    }
    for (struct addrinfo *pa = server_info; pa != NULL; pa = pa->ai_next) {
        struct sockaddr_in *addr = (struct sockaddr_in *)pa->ai_addr;
        if ((sock = socket(pa->ai_family, pa->ai_socktype, pa->ai_protocol)) <
            0) {
            perror(INFO "Retrying socket creation");
            continue;
        }
        // reuse address
        {
            int yep = 1;
            if ((ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yep,
                                  sizeof(int))) != 0) {
                perror(INFO "Set reuse address error, retrying");
                continue;
            }
        }
        evutil_make_socket_nonblocking(sock);

        char addr_str[INET6_ADDRSTRLEN];
        void *in_addr = &(((struct sockaddr_in *)addr)->sin_addr);
        inet_ntop(AF_INET, in_addr, addr_str, INET_ADDRSTRLEN);
        printf(INFO "Bind socket to address %s\n", addr_str);

        write_args.send_addr = (struct sockaddr *)addr;
        write_args.send_addr_len = sizeof(struct sockaddr_in);
        break;
    }
    // if error occurs
    if (sock < 0) {
        REPORT_ERRNO("socket");
    }
    if (ret != 0) {
        REPORT_ERRNO("DEAD");
    }

    // initialize write event
    base = event_base_new();
    if (!base) {
        ret = -1;
        fprintf(stderr, ERROR "Event base creation error\n");
        goto defer;
    }
    write_event = event_new(base, sock, EV_WRITE | EV_PERSIST, write_callback,
                            &write_args);
    write_args.self = write_event;
    event_add(write_event, NULL);
    event_base_dispatch(base);

defer:
    if (server_info) {
        freeaddrinfo(server_info);
    }
    if (sock > -1) {
        close(sock);
    }
    return ret;
}

static void write_callback(evutil_socket_t sock, [[maybe_unused]] short what,
                           void *arg) {
    struct write_event_arguments *write_args =
        (struct write_event_arguments *)arg;
    assert(write_args != NULL);
    printf("\x1b[90m>>>\x1b[0m ");
    // size_t line_len = 0;
    char *line = fgets(write_args->write_buff, WRITE_BUFF_LEN, stdin);
    if (line) {
        ssize_t send_len =
            sendto(sock, line, strlen(line), 0, write_args->send_addr,
                   write_args->send_addr_len);
        printf("Sent %zu bytes\n", send_len);
    } else if (feof(stdin)) {
        event_del(write_args->self);
    }
    // TODO: do I handle ferror
}
