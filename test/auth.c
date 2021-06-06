#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../util.h"

int authenticate(char *formatted_creds);

int main(void)
{
    int sockfd = make_server(8001, 5, BLOCK);
    char buffer[1024];

    while (1) {
        int clientfd = accept_connection(sockfd, BLOCK);
        recv_wrapper(clientfd, buffer, sizeof(buffer), "auth-recv-creds");
        int ok = authenticate(buffer);
        send_wrapper(clientfd, (ok ? "true" : "false"), "auth-send-auth-status");
        close(clientfd);
    }

    close(sockfd);

    return 0;
}

int authenticate(char *formatted_creds)
{
    static char lines[100][1024] = { 0 };
    static int db_loaded = 0;
    const char passwd_file[] = "authdb/passwd";

    if (!db_loaded) {
        if (access(passwd_file, F_OK) == 0) {
            FILE *file = fopen(passwd_file, "r");
            if (file) {
                int i = 0;
                while (fgets(lines[i], sizeof(lines[i]), file)) {
                    lines[i][strlen(lines[i])-1] = '\0';
                    i++;
                }
            }
            fclose(file);
            db_loaded = 1;
        }
    }

    for (int i = 0; lines[i] != NULL; i++) {
        if (strcmp(lines[i], formatted_creds) == 0) {
            return 1;
        }
    }

    return 0;
}
