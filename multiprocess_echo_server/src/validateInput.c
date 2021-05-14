#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

extern char *optarg;

void serverHelp(char *argvZero) {
    printf("Usage: %s -p [PORT]\n", argvZero);
}

void clientHelp(char *argvZero) {
    printf("Usage: %s -a [IP] -p [PORT]\n", argvZero);
}

bool isValidPortNumber(char *strPort) {
    int len = strlen(strPort);
    if (len > 5) return false;
    for (int i=0; i<len; i++) {
        if (isdigit(strPort[i]) == false) return false;
    }

    int numPort = atoi(strPort);
    if (numPort < 0 || numPort > 65535) return false;
    return true;
}

bool isValidIpAddress(char *ipAddress) {
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return result != 0;
}

bool validateInputServer(int argc, char *argv[], char **port) {
    if (argc != 3) {
        serverHelp(argv[0]);
        return false;
    }

    int c = -1;
    while (true) {
        c = getopt(argc, argv, "hp:");
        if (c == -1) break;

        switch (c) {
            case 'h': 
                serverHelp(argv[0]);
                break;
            case 'p': 
                if (isValidPortNumber(optarg)) {
                    *port = optarg;
                    return true;
                }
                else {
                    fprintf(stderr, "%s is not port\n", optarg);
                    return false;
                }
        }
    }
    return false;
}

bool validateInputClient(int argc, char *argv[], char **ip, char **port) {
    bool checkIp = false;
    bool checkPort = false;

    if (argc != 5) {
        clientHelp(argv[0]);
        return false;
    }

    int c = -1;

    while (true) {
        c = getopt(argc, argv, "ha:p:");
        if (c == -1) break;

        switch (c) {
            case 'h': 
                clientHelp(argv[0]);
                break;
            case 'a': 
                printf("ip address = %s\n", optarg);
                if (isValidIpAddress(optarg)) {
                    checkIp = true;
                    *ip = optarg;
                    break;
                }
                else {
                    fprintf(stderr, "%s is not ip\n", optarg);
                    return false;
                }
            case 'p': 
                printf("port =  %s\n", optarg);
                if (isValidPortNumber(optarg)) {
                    checkPort = true;
                    *port = optarg;
                    break;
                }
                else {
                    fprintf(stderr, "%s is not port\n", optarg);
                    return false;
                }
        }
    }
    if (checkIp && checkPort) return true;
    return false;
}