#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX 1024

// Function to read a line from the socket using recv
ssize_t readline(int socket_fd, char *buffer, size_t maxlength) {
    ssize_t read_inp = 0;
    char c;
    ssize_t n;

    while (read_inp < maxlength - 1) {
        n = recv(socket_fd, &c, 1, 0);
        //printf("Byte received from server: %zd\n", n);
        if (n <= 0) {
            perror("connection closed here");
            return -1;
        }
        
        buffer[read_inp] = c;
        read_inp++;
        if (c == '\n') {
            break;
        }
    }
    buffer[read_inp] = '\0';
    return read_inp;
}

// Function to write n bytes to the socket using send
ssize_t writen(int socket_fd, const char *buffer, ssize_t n) {
    ssize_t write_out = 0;
    ssize_t bytes_written;

    while (write_out < n) {
        bytes_written = send(socket_fd, buffer + write_out, n - write_out, 0);
        if (bytes_written < 0) {
            perror("Send error");
            return -1;  // error encountered here
        }
        printf("Total no of Bytes written: %zd\n", bytes_written);
        write_out += bytes_written;
    }
    return write_out;
}

// Function to handle read and write operation with server
void read_write(int socket_fd) {
    char buffer[MAX+1];
    ssize_t n;

    printf("Please Enter String (or press Ctrl+D to quit):");
    
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if (writen(socket_fd, buffer, strlen(buffer)) < 0) {
            perror("Write error");
            break;
        }
        
        n = readline(socket_fd, buffer, sizeof(buffer) );
        if (n <= 0) {
            perror("Read error");
            break;
        }
        
        buffer[n] = '\0';
        printf("Echo from server: %s\n", buffer);
        printf("Please Enter String (or press Ctrl+D to quit): ");
        
    }
    printf("\nSocket closing now.\n");  // Detected EOF
    
    close(socket_fd);
}

int main(int argc, char *argv[]) {

    const char *ip_address = argv[1];
    int port = atoi(argv[2]);
    int socket_fd;
    ssize_t n;
    struct sockaddr_in server_address;
    
    // Create a socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Set up server address
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    
    // Check address
    if (inet_pton(AF_INET, ip_address, &server_address.sin_addr) <= 0) {
        perror("Wrong address");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    
    // Build connection with server
    n = connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address));
    if (n < 0) {
        perror("Connect to server failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server at %s:%d\n", ip_address, port);
    // Call the function to perform read and write
    read_write(socket_fd);
    close(socket_fd);
    return 0;
}
