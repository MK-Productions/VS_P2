//
// Created by Markus Kapp on 20.04.21.
//

/*
* File:   daytime.c
* Author: Heinz-Josef Eikerling
*
*         Implementierung des Daytime-Service.
*		   Compile: gcc -o daytime daytime.c
*		   Aufruf: daytime oder daytime <host address> <host port>
*
* Created on 10. M�rz 2012, 21:54
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>

/*
 * Fehler ausgeben und Programm beenden.
 */
static void handleError(const char *cause) {
    if (errno != 0) {
        fputs(strerror(errno), stderr);
        fputs(": ", stderr);
    }
    fputs(cause, stderr);
    fputc('\n', stderr);
    exit(1);
}

/*
 * main-Funktion
 */
int main(int argc, char **argv) {
    int z;
    char *srvr_addr = "127.0.0.1";
    char *srvr_port = "8080";
    struct sockaddr_in adr_srvr; // AF_INET
    struct sockaddr_in adr_clnt; // AF_INET
    int addr_len;                // length
    int s;                       // Socket
    int c;                       // Client socket
    int n;                       // bytes
    time_t td;                   // Current date&time
    char *docroot;
    char tstamp[128];

    /*
    * Server-Adresse von der Kommandozeile lesen,
    * falls vorhanden.
    */
    if (argc >= 2) {
        docroot = argv[1]; // Pfad zur html file
    } else {
        docroot = "docroot_2/folder/index.html";  // Use default
    }

    /*
    * Server-Port von der Kommandozeile lesen,
    * falls vorhanden.
    */
    if (argc >= 3) {
        srvr_port = argv[2];
    }

    /*
    * Socket anlegen.
    */
    s = socket(PF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        handleError("socket()");
    }
    memset(&adr_srvr, 0, sizeof adr_srvr);
    adr_srvr.sin_family = AF_INET;
    adr_srvr.sin_port = htons(atoi(srvr_port));
    if (strcmp(srvr_addr, "*") != 0) {
        adr_srvr.sin_addr.s_addr = inet_addr(srvr_addr); // Normal Address
        if (adr_srvr.sin_addr.s_addr == INADDR_NONE) {
            handleError("bad address.");
        }
    } else {
        adr_srvr.sin_addr.s_addr = INADDR_ANY; // Wild Address
    }

    /* Adresse an Server binden. */
    addr_len = sizeof adr_srvr;
    z = bind(s, (struct sockaddr *) &adr_srvr, addr_len);
    if (z == -1) {
        handleError("bind(2)");
    }

    z = listen(s, 10);
    if (z == -1) {
        handleError("listen(2)");
    }

    while (true) {
        /* Server Loop. Auf Anfragen warten und diese auswerten */
        addr_len = sizeof adr_clnt;
        c = accept(s, (struct sockaddr *) &adr_clnt, &addr_len);

        if (c == -1) {
            handleError("accept(2)");
        }

        /* Zeitstempel erzeugen */
        time(&td);
        n = (int) "lul";

//        /* Ergebnis an Client geben */
        z = write(c, tstamp, n);
        if (z == -1) {
            handleError("write(2)");
        }

        printf("lul");

        /* Verbindung schlie�en */
        close(c);
    }
    return 0;
}