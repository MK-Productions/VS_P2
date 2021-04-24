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

#define MAXLINE 4096
#define SOCKET_ADDRESS struct sockaddr
#define true 1
#define false 0

int serverPort = 8080;

void printError(const char *errorMessage) {
    printf("%s \n", errorMessage);
    exit(1);
}

void exitHandler(int port) {
    close(serverPort);
    printf("\nClosed port!\n");
    exit(0);
}

const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

int main(int argc, char **argv) {
    int listenFD, connFD, n;
    struct sockaddr_in servaddr;
    uint8_t buff[MAXLINE + 1];
    uint8_t receiveLine[MAXLINE + 1];
    char docroot[MAXLINE];
    char tstamp[128];            // Date/Time info
    time_t td;                   // Current date&time

    if (argc >= 2) {
        strcpy(docroot, argv[1]);
    } else {
        strcpy(docroot, "docroot_2/folder/"); // Use default directory
    }

    if (argc >= 3) {
        serverPort = atoi(argv[2]);
    }

    // create socket
    if ((listenFD = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printError("socket error!\n");
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(serverPort);

    if ((bind(listenFD, (SOCKET_ADDRESS *) &servaddr, sizeof(servaddr))) < 0) {
        printError("bind error!");
    }

    if ((listen(listenFD, 10)) < 0) {
        printError("listen error!");
    }

    printf("Succesfully created TCP Socket listening %d %s", serverPort, "!\n");
    fflush(stdout);

    signal(SIGINT, exitHandler);

#pragma ide diagnostic ignored "EndlessLoop"
    while (true) {
        connFD = accept(listenFD, (SOCKET_ADDRESS *) NULL, NULL);
        struct stat sb;

        char serverReply[MAXLINE];
        recv(connFD, serverReply, MAXLINE, 0);

        if (strstr(serverReply, "favicon") != NULL) {
            continue;
        }

        char path[100];
        for (int i = 5; i < MAXLINE; i++) {
            if (serverReply[i] == ' ') {
                break;
            }

            path[i - 5] = serverReply[i];
        }

        // read html file contents
        char completePath[100];
        strcat(completePath, docroot);
        strcat(completePath, path);

        memset(path, 0, 100);

        char response[MAXLINE];
        char *body;

        if (access(completePath, F_OK) < 0) {
            strcat(response, "HTTP/1.1 404 Not Found\n");
            body = "404 not found";
        } else {

            // read file
            FILE *fp = fopen(completePath, "rb");

            stat(completePath, &sb);
            // fstat(fd, &sb);

            body = malloc(sb.st_size);

            fread(body, sb.st_size, 1, fp);

            fclose(fp);

            time(&td);
            strftime(tstamp, sizeof tstamp, "Date: %a, %d %b %G %T %Z\n", localtime(&td));

            char lastModified[50];
            strftime(lastModified, 50, "Last-modified: %a, %d %b %G %T %Z\n", localtime(&sb.st_mtime));

            // status 200 OK Header
            strcat(response, "HTTP/1.1 200 OK\n");
            strcat(response, tstamp);
            strcat(response, lastModified);
            strcat(response, "Content-Language: de\n");

            if (strcmp(get_filename_ext(completePath), "jpg") == 0) {
                strcat(response, "Content-Type: image/jpeg\n");
                strcat(response, "Content-Length: 20896\r\n\r\n");
            } else {
                strcat(response, "Content-Type: text/html; charset=iso8859-1\r\n\r\n");
            }
        }

        memset(completePath, 0, 100);

//        strcat(response, body);


        // send response
//        write(connFD, response, sizeof(char) * strlen(response));
        write(connFD, body, sb.st_size);
        memset(response, 0, MAXLINE);
        close(connFD);
    }
}

/*
 * TODO:
 *      - docroot und port variable machen ✅
 *      - aufgerufene Datei muss ausgelsen werden und in content gespeichert werden ✅
 *          - datei auslesen muss in die schleife ✅
 *          - falls Datei nicht gefunden wird 404 not found zurückgeben ✅
 *          - Fehlerhandling beim Datei auslesen ✅
 *      - Header vervollständigen ✅
 *      - Bilder darstellen binär
 *          - Content-Type binär
 *      - printf, snprint durch write ersetzen
 *      - nur verarbeitung GET Requests nachgucken
 *      - mehrstufiger Server (fork i guess)
 *      - Hinweise nochmal checken
 *          - warum kompilieren mit -g
 *          - valgrind
 */

