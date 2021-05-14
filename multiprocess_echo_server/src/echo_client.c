#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "validateInput.h"

#define BUF_SIZE 1024

void error_handling(char *msg) {
    fprintf(stderr, "%s \n", msg);
    exit(1);
}

// 부모프로세스 담당
void read_routine(int sock, char *buf) {
    while (true) {
        int str_len = read(sock, buf, BUF_SIZE);
        if (str_len == 0) return;

        buf[str_len] = 0;
        printf("from server: %s", buf);
    }
}

// 자식 프로세스 담당
void write_routine(int sock, char *buf) {
    while (true) {
        fgets(buf, BUF_SIZE, stdin); // 사용자 입력스트림을 가져온닽
        if(!strcmp(buf,"q\n") || !strcmp(buf,"Q\n")) {
            shutdown(sock, SHUT_WR);
            return;
        }
        write(sock, buf, strlen(buf));
    }
}

int main(int argc, char *argv[]) {
    char *ip = "";
    char *port = "";

    bool check = validateInputClient(argc, argv, &ip, &port);
    if (check == false) {
        exit(1);
    }

    // 소켓 생성
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        error_handling("socket() error");
    }

    // 소켓 연결
    struct sockaddr_in serv_addr = { 0, };
    serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(ip);
	serv_addr.sin_port=htons(atoi(port));
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("connect() error");
    }
    else {
        printf("connect success\n");
    }

    pid_t pid = fork();
    char buf[BUF_SIZE] = { 0, };
    if (pid == 0) {
        write_routine(sock, buf);
    }
    else {
        read_routine(sock, buf);
    }

    close(sock);
    return 0;
}