#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../util.h"

#define MAX_BUFFER 1024

void prompt_login_details(char *username, char *password, int size);
void make_credentials_buffer(char *buffer, char *username, char *password);
void prompt_command(char *cmd, int size);

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "HOSTNAME required.\n");
        exit(255);
    }

    int serv_fd = make_client(argv[1], 8000, BLOCK);

    char username[32] = { 0 };
    char password[32] = { 0 };
    prompt_login_details(username, password, sizeof(password));

    char buffer[MAX_BUFFER] = { 0 };
    make_credentials_buffer(buffer, username, password);

    send_wrapper(serv_fd, buffer, "cl-send-creds");
    recv_wrapper(serv_fd, buffer, sizeof(buffer), "cl-recv-auth-status");

    if (strcmp(buffer, "true") == 0) {
        fprintf(stderr, "Clear to go.\n");
    } else {
        fprintf(stderr, "Wrong username or password.\n");
        close(serv_fd);
        exit(250);
    }

    char cmd[1024] = { 0 };

    while (1) {
        prompt_command(cmd, sizeof(cmd));
        if (strlen(cmd) > 0) {
            send_wrapper(serv_fd, cmd, "cl-send-cmd");
            recv_wrapper(serv_fd, buffer, sizeof(buffer), "cl-recv-resp");
            printf("%s\n", buffer);
        }
    }

    close(serv_fd);

    return 0;
}

void prompt_login_details(char *username, char *password, int size)
{
    printf("Enter username: ");
    fgets(username, size, stdin);
    username[strlen(username)-1] = '\0';
    printf("Enter password: ");
    fgets(password, size, stdin);
    password[strlen(password)-1] = '\0';
}

void make_credentials_buffer(char *buffer, char *username, char *password)
{
    strncat(buffer, username, strlen(username));
    strncat(buffer, "\t", 1);
    strncat(buffer, password, strlen(password));
}

void prompt_command(char *cmd, int size)
{
    memset(cmd, 0, size);
    printf("store> ");
    fflush(stdout);
    fgets(cmd, size, stdin);
    cmd[strlen(cmd)-1] = '\0';
}
