#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "util.h"

void auth_exchange(int clientfd, int authfd, char *buf, int bufsz)
{
    recv_wrapper(clientfd, buf, bufsz, "lb-recv-creds");
    send_wrapper(authfd, buf, "lb-send-creds");

    recv_wrapper(authfd, buf, bufsz, "lb-recv-auth-status");
    send_wrapper(clientfd, buf, "lb-send-auth-status");
}

void storage_exchange(int clientfd, int storefd, char *buf, int bufsz)
{
    while (1) {
        // Always send 1
        recv_wrapper(clientfd, buf, bufsz, "lb-recv-cmd");
        send_wrapper(storefd, buf, "lb-send-cmd");

        char *result = strstr(buf, "send");

        if (result && (result - buf == 0)) {
            // Send 2 more if the file is being sent
            recv_wrapper(storefd, buf, bufsz, "lb-recv-resp");
            send_wrapper(clientfd, buf, "lb-send-resp");

            recv_wrapper(storefd, buf, bufsz, "lb-recv-resp");
            send_wrapper(clientfd, buf, "lb-send-resp");
        } else {
            // But receive 2 if the file is being received
            recv_wrapper(storefd, buf, bufsz, "lb-recv-resp");
            send_wrapper(clientfd, buf, "lb-send-resp");

            recv_wrapper(storefd, buf, bufsz, "lb-recv-resp");
            send_wrapper(clientfd, buf, "lb-send-resp");
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s auth storage\n", argv[0]);
        exit(255);
    }

    int sockfd = make_server(8000, 5, BLOCK);
    char buffer[1024];

    while (1) {
        int clientfd = accept_connection(sockfd, BLOCK);
        int authfd = make_client(argv[1], 8001, BLOCK);

        auth_exchange(clientfd, authfd, buffer, sizeof(buffer));
        close(authfd);

        int storefd = make_client(argv[2], 8002, BLOCK);

        while (1) {
            storage_exchange(clientfd, storefd, buffer, sizeof(buffer));
        }

        close(storefd);
        close(clientfd);
    }

    close(sockfd);

    return 0;
}
