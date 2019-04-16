#include <time.h>
#include <stdio.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

int init();
void start(int);
void get_time(char t[]);
void time_spam(int);

int main(int argc, char * argv[]) {
    int sock;

    sock = init();
    if(sock < 0) return(0);
    start(sock);

    return 0;
}

int init() {
    //Create socket
    char time_wrap[10];
    get_time(time_wrap);
    printf("%s: Creating socket\n", time_wrap);
    usleep(100000);

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        perror("Can't create socket");
        return -1;
    }

    int optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    get_time(time_wrap);
    printf("%s: Successful socket created\n", time_wrap);
    usleep(100000);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1010);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    int bind_status = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (bind_status < 0) {
        perror("Bind");
        return -1;
    }

    get_time(time_wrap);
    printf("%s: Successful bind\n", time_wrap);
    usleep(100000);

    int listen_status = listen(sock, SOMAXCONN);
    if (listen_status < 0) {
        perror("Listen");
        return -1;
    }

    get_time(time_wrap);
    printf("%s: Server is ready\n", time_wrap);
    usleep(100000);

    return sock;
}

void start(int sock) {
    char time_wrap[10];
    while(1) {
        int slave_socket;
        struct sockaddr_in accept_addr;
        int size = sizeof(accept_addr);
        slave_socket = accept(sock, (struct sockaddr *)&accept_addr, (socklen_t*)&size);
        if (slave_socket < 0) {
            perror("Accept");
            continue;
        }


        get_time(time_wrap);
        printf("%s: New Connection: %s\n", time_wrap, inet_ntoa(accept_addr.sin_addr));

        time_spam(slave_socket);
        shutdown(slave_socket, SHUT_RDWR);
        close(slave_socket);

        get_time(time_wrap);
        printf("%s: Connection lost: %s\n", time_wrap, inet_ntoa(accept_addr.sin_addr));
    }
}

void time_spam(int s) {
    size_t total, length = 10;
    ssize_t sent;
    char time_wrap[10];

    while(1) {
        get_time(time_wrap);
        sent = 0;
        total = 0;
        while (total < length) {
            sent = send(s, &time_wrap, length, MSG_NOSIGNAL);
            if (sent == -1) {
                perror("Send");
                return;
            }
            total += sent;
        }
        sleep(1);
    }
}

void get_time(char time_wrap[]) {
    struct tm timeval;
    time_t timer = time(0);
    timeval = *localtime(&timer);
    sprintf(time_wrap, "%.2d:%.2d:%.2d", timeval.tm_hour, timeval.tm_min, timeval.tm_sec);
    time_wrap[9] = '\0';
}