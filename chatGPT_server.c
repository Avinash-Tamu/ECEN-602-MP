// echo_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    while ((bytes_received = read(client_socket, buffer, sizeof(buffer))) > 0) {
        // Echo back the received data
        write(client_socket, buffer, bytes_received);
    }

    close(client_socket);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create a socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address struct
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind the socket to the address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) < 0) {
        perror("listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", port);

    while (1) {
        // Accept a client connection
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
            perror("accept");
            continue;
        }

        printf("Client connected\n");

        // Fork a child process to handle the client
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            close(server_socket);
            handle_client(client_socket);
        } else if (pid > 0) {
            // Parent process
            close(client_socket);
            // Clean up zombie processes
            waitpid(-1, NULL, WNOHANG);
        } else {
            perror("fork");
            close(client_socket);
        }
    }

    close(server_socket);
    return 0;
}
