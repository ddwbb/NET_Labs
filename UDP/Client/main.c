#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

int init_socket();
struct sockaddr_in init_addr();
void main_loop(int, struct sockaddr_in);


int main() {
    int sock;
    struct sockaddr_in addr;

    sock = init_socket();
    if (sock == -1) return 0;

    addr = init_addr();
    main_loop(sock, addr);

    close(sock);

    return 1;
}

int init_socket() {
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        perror("Socket creating");
        return -1;
    }

    struct timeval timeout;
    timeout.tv_sec = 10;
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

    return sock;
}

struct sockaddr_in init_addr() {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    return addr;
}

void main_loop(int sock, struct sockaddr_in addr) {
    ssize_t size = 0;
    socklen_t length = sizeof(addr);
    char * buffer = (char *)calloc(10, sizeof(char));
    buffer[0] = '\0';
    for (int i = 0; i < 10; i++) {
        sendto(sock, buffer, 10, MSG_NOSIGNAL, (struct sockaddr *)&addr, length);
        size = recvfrom(sock, buffer, 10, MSG_NOSIGNAL, NULL, NULL);
        if (size < 0) {
            perror("Server is busy");
            return;
        }
        printf("Receive from Server: %s\n", buffer);
        sleep(1);
    }
}









