#include <stdio.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

int make_connection();
int get_message(int);

int main(int argc, char * argv[]) {
    int sock;
    sock = make_connection();
    if (sock == -1) return 0;

    int sock_status;
    for (int i = 0; i < 5; i++) {
        sock_status = get_message(sock);
        if (sock_status == -1) {
            shutdown(sock, SHUT_RDWR);
            close(sock);
            break;
        }
    }

    return 0;
}

int make_connection() {
    int sock;
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        perror("Socket create");
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1010);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("Connect");
        return -1;
    }

    return sock;
}

int get_message(int s) {
    char message[10];
    ssize_t received = 0;
    size_t length, total = 0;
    length = 10;

    while(total < length) {
        received = recv(s, message, length, MSG_NOSIGNAL);
        if (received == -1) {
            perror("Receive");
            return -1;
        }
        total += received;
    }

    printf("Received from server: %s\n", message);
    return 0;
}