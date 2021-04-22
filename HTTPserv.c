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

#define SERVER_PORT 8080
#define MAXLINE 4096
#define SOCKET_ADDRESS struct sockaddr

void printError(const char *errorMessage) {
    printf("%s", errorMessage);
}

int main(int argc, char **argv) {
    int sockfd;
    int n;
    int sendBytes;
    struct sockaddr_in servaddr;
    char sendLine[MAXLINE];
    char receiveLine[MAXLINE];

    if (argc != 2) {
        printError("server address error");
    }

    // create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printError("Error while creating socket");
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        printError("inet_pton error");
    }

    // connect to server
    if (connect(sockfd, (SOCKET_ADDRESS *) &servaddr, sizeof(servaddr)) < 0) {
        printError("connect failed!");
    }

    sprintf(sendLine, "GET / HTTP/1.1\r\n\r\n");
    sendBytes = strlen(sendLine);

    if (write(sockfd, sendLine, sendBytes) != sendBytes) {
        printError("write error");
    }

    // read server response
    while ((n = read(sockfd, receiveLine, MAXLINE-1)) > 0) {
        printf("%s", receiveLine);

        memset(receiveLine, 0, MAXLINE);
    }

    if (n < 0) {
        printError("read error");
    }

    exit(0);
}
