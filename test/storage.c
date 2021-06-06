#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include "../util.h"

int found(char *filename);
void list_files(char *buffer);
void command_handler(int clientfd, char *buffer, int bufsz);

int main(void)
{
    int sockfd = make_server(8002, 5, BLOCK);

    while (1) {
        int clientfd = accept_connection(sockfd, BLOCK);
        while (1) {
            command_handler(clientfd, cmd, sizeof(cmd));
        }
        close(clientfd);
    }

    close(clientfd);
    close(sockfd);

    return 0;
}

void make_path(char *path, char *filename)
{
    sprintf(path, "storedb/%s", filename);
}

int found(char *filename)
{
    char pathname[strlen(filename)+9];
    sprintf(pathname, "storedb/%s", filename);

    if (access(pathname, F_OK) == 0) {
        return 1;
    }

    return 0;
}

void list_files(char *buffer)
{
    DIR *curdir;
    struct dirent *ent;

    if ((curdir = opendir("storedb")) != NULL) {
        while ((ent = readdir(curdir)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 ||
                    strcmp(ent->d_name, "..") == 0) {
                continue;
            }
            printf("%s\n", ent->d_name);
            strncat(buffer, ent->d_name, strlen(ent->d_name));
            strncat(buffer, "\n", 1);
        }
        buffer[strlen(buffer)-1] = '\0';
        closedir(curdir);
    }
}

void command_handler(int clientfd)
{
    char buffer[1024] = { 0 };
    char cmd[1024] = { 0 };
    char path[1024] = { 0 };

    recv_wrapper(clientfd, cmd, sizeof(cmd), "store-recv-cmd");
    memset(path, 0, sizeof(path));
    if (strcmp(cmd, "ls") == 0) {
        memset(buffer, 0, sizeof(buffer));
        //sprintf(path, "storedb");
        list_files(buffer);
        send_wrapper(clientfd, buffer, "store-send-resp");
    } else if (strcmp(cmd, "dl hello.txt") == 0) {
        if (found("hello.txt")) {
            //sprintf(path, "storedb/%s", filename);
            make_buffer(buffer, "hello.txt");
            send_wrapper(clientfd, buffer, "store-send-file");
        }
    } else if (strcmp(cmd, "ul aloha.txt") == 0) {
        recv_wrapper(clientfd, buffer, sizeof(buffer), "store-recv-file");
        make_file(buffer, "aloha.txt");
    } else if (strcmp(cmd, "quit") == 0) {
        return;
    }
}
