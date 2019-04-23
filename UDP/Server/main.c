#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

int init();
void print_action(const char *);
void start(int);
char * get_time();

int main() {
    int sock;
    sock = init();
    if (sock) {
        print_action("Server is ready");
        start(sock);
    }
    return 0;
}

int init() {
    int sock;
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sock < 0) {
        perror("Socket creating");
        return -1;
    }
    print_action("Successful socket creation");
    usleep(100000);

    int optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(8080);
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind");
        return -1;
    }
    print_action("Successful bind");
    usleep(100000);

    return sock;
}

char * get_time() {
    char * time_wrap;
    time_wrap = (char *)calloc(9, sizeof(char));
    if (time_wrap == NULL) return NULL;
    struct tm timeval;
    time_t timer = time(0);
    timeval = (*localtime(&timer));
    sprintf(time_wrap, "%.2d:%.2d:%.2d", timeval.tm_hour, timeval.tm_min, timeval.tm_sec);
    time_wrap[8] = '\0';
    return time_wrap;
}

void print_action(const char * action) {
    printf("%s: %s\n", get_time(), action);
}

void start(int sock) {
    struct sockaddr_in addr;
    socklen_t length = sizeof(addr);
    char * buffer = (char *)calloc(10, sizeof(char));

    while(1) {
        recvfrom(sock, buffer, 10, MSG_NOSIGNAL, (struct sockaddr *) &addr, &length);
        buffer = get_time();
        sendto(sock, buffer, 10, MSG_NOSIGNAL, (struct sockaddr *) &addr, length);
    }
}