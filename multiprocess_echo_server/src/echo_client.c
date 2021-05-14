#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "validateInput.h"
#include <signal.h>
#include <sys/wait.h>

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
    printf("client shutdown\n");
    exit(0);
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
int write_routine(int sock, char *buf) {
    while (true) {  
        fgets(buf, BUF_SIZE, stdin); // 사용자 입력스트림을 가져온다
        if(!strcmp(buf,"q\n") || !strcmp(buf,"Q\n")) {
            // shutdown(sock, SHUT_WR);
            close(sock);
            return 0;
        }

        write(sock, buf, strlen(buf)); // 소켓으로 전송한다
    }
}

int main(int argc, char *argv[]) {    
    char *ip = "";
    char *port = "";

    bool check = validateInputClient(argc, argv, &ip, &port);
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
        int childV = write_routine(sock, buf);
        return childV;
    }
    else {
        read_routine(sock, buf);
    }

    close(sock);
    return 0;
}