// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IPAdr> <Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *ip_addr = argv[1];
    int port = atoi(argv[2]);
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[MAX];
    ssize_t n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip_addr, &servaddr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server at %s:%d\n", ip_addr, port);
    printf("Enter String:");
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        
        write(sockfd, buffer, strlen(buffer));
        n = read(sockfd, buffer, sizeof(buffer) - 1);
        if (n <= 0) {
            perror("Read error or server closed connection");
            break;
        }
        buffer[n] = '\0';  // Null-terminate the received data
        printf("Echo from server: %s", buffer);
        printf("Enter String:");
    }

    close(sockfd);
    return 0;
}
