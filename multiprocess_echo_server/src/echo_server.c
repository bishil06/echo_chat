#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "validateInput.h"

#define BUF_SIZE 1024

void error_handling(char *msg) {
    fprintf(stderr, "%s \n", msg);
    exit(1);
}

void endChildAction(int sig) {
    int status = 0;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status)) {
        printf("pid = %d child process success\n", pid);
    }
    else {
        printf("pid = %d child process fail\n", pid);
    }
    printf("child process return %d\n", WEXITSTATUS(status));
}

int main(int argc, char *argv[]) {
    char *port = "";
    bool check = validateInputServer(argc, argv, &port);
    if (check == false) {
        exit(1);
    }

    // 자식프로세스 처리 시그널 추가
    struct sigaction act = { 0, };
    act.sa_handler = endChildAction;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    int state = sigaction(SIGCHLD, &act, 0);

    // 소켓 생성
    int serv_sock = -1;
    serv_sock=socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) {
        error_handling("socket() error");
    }
    // 소켓 주소 설정
    struct sockaddr_in serv_addr = { 0, };
    serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(port));
    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }

    if (listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }

    printf("server listenning %s port\n", port);

    while (true) {
        struct sockaddr_in clnt_addr = { 0, };
        socklen_t clnt_addr_size = sizeof(clnt_addr);
        int clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1) {
            fprintf(stderr, "accpet() error\n");
            continue;
        }
        else {
            printf("new client connected ...\n");
        }

        pid_t pid = fork();
        if (pid == -1) {
            close(clnt_sock);
            continue;
        }
        if (pid == 0) {
            close(serv_sock);
            int str_len = 0;
            char buf[BUF_SIZE] = { 0, };
            while (true) {
                str_len = read(clnt_sock, buf, BUF_SIZE);
                if (str_len == 0) {
                    break;
                }

                write(clnt_sock, buf, str_len);
            }
            close(clnt_sock);
            printf("client disconnected ...\n");
            return 0;
        }
    }
    
    return 0;
}