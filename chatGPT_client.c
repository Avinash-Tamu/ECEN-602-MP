// echo_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <Server IP> <Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *server_ip = argv[1];
    int port = atoi(argv[2]);
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_sent, bytes_received;

    // Create a socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address struct
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server. Type messages to send (Ctrl-D to exit):\n");

    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        // Send the message to the server
        if ((bytes_sent = write(client_socket, buffer, strlen(buffer))) < 0) {
            perror("write");
            break;
        }

        // Receive the echoed message from the server
        if ((bytes_received = read(client_socket, buffer, BUFFER_SIZE)) < 0) {
            perror("read");
            break;
        }

        // Print the echoed message
        buffer[bytes_received] = '\0';
        printf("Received from server: %s", buffer);
    }

    close(client_socket);
    return 0;
}
