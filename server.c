// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define MAX 1024

void handle_client(int connfd) {
    char buffer[MAX];
    ssize_t n;

    while ((n = read(connfd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';  // Null-terminate the received data
        printf("Received from client: %s", buffer);
        write(connfd, buffer, n);  // Echo back to client
    }

    if (n < 0) {
        perror("Read error");
    }

    close(connfd);
    exit(0);  // Exit the child process
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    int sockfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);
    pid_t pid;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 5) < 0) {
        perror("Listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", port);

    while (1) {
        connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
        if (connfd < 0) {
            perror("Server accept failed");
            continue;
        }

        if ((pid = fork()) == 0) {  // Create a child process to handle the client
            close(sockfd);
            handle_client(connfd);
        } else if (pid > 0) {
            close(connfd);
            waitpid(-1, NULL, WNOHANG);  // Clean up finished child processes
        } else {
            perror("Fork failed");
        }
    }

    close(sockfd);
    return 0;
}
