//
// Created by Markus Kapp on 20.04.21.
//

#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <sys/stat.h>


#define SERVER_PORT 8080
#define MAXLINE 4096
#define SOCKET_ADDRESS struct sockaddr
#define true 1
#define false 0

void printError(const char *errorMessage) {
    printf("%s \n", errorMessage);
    exit(1);
}

// Quelle: https://stackoverflow.com/questions/8465006/how-do-i-concatenate-two-strings-in-c/8465083
char* concat(const char *s1, const char *s2)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = malloc(len1 + len2 + 1);

    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1);

    return result;
}

int main(int argc, char **argv) {
    int listenFD, connFD, n;
    struct sockaddr_in servaddr;
    uint8_t buff[MAXLINE + 1];
    uint8_t receiveLine[MAXLINE + 1];

    // read html file contents
    int fd = open("docroot_2/folder/index.html", O_RDONLY);
    struct stat sb;

    fstat(fd, &sb);
    char *contents = calloc(1, sb.st_size + 1);
    read(fd, contents, sb.st_size);
    close(fd);

    // create socket
    if ((listenFD = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printError("socket error!");
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT);

    if ((bind(listenFD, (SOCKET_ADDRESS *) &servaddr, sizeof(servaddr))) < 0) {
        printError("bind error!");
    }

    if ((listen(listenFD, 10)) < 0) {
        printError("listen error!");
    }

#pragma ide diagnostic ignored "EndlessLoop"
    while (true) {
        struct sockaddr_in address;
        socklen_t addr_len;

        connFD = accept(listenFD, (SOCKET_ADDRESS *) NULL, NULL);
        memset(receiveLine, 0, MAXLINE);

        while ((n = read(connFD, receiveLine, MAXLINE - 1)) > 0) {
            if (receiveLine[n - 1] == '\n') {
                break;
            }

            memset(receiveLine, 0, MAXLINE);
        }

        if (n < 0) {
            printError("read error!");
        }

        // send response
        char* response = concat("HTTP/1.1 200 OK\r\n\r\n", contents);
        snprintf((char *) buff, sizeof(buff), "%s", response);

        write(connFD, (char *) buff, strlen((char *) buff));
        close(connFD);
    }
}
