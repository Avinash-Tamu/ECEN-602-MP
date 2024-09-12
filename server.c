#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define MAX 1024

// readline- func to read a data from socket
ssize_t readline(int connect_fd, char *buffer, size_t maxlength) {
    ssize_t read_inp = 0;
    char d;
    ssize_t n;

    while (read_inp < maxlength - 1) {
        n = recv(connect_fd, &d, 1, 0);
        //printf("Byte received from client: %zd\n", n);
        if (n < 0) {
            perror("Receive error");
            return -1;
        }
        else if (n == 0) {
            break;
        }
        buffer[read_inp] = d;
        read_inp++;
        if (d == '\n') {
            break;
        }
    }
    buffer[read_inp] = '\0';
    return read_inp;
}

// writen - func to write n bytes data to socket
ssize_t writen(int connect_fd, const char *buffer, ssize_t n) {
    ssize_t write_out = 0;
    ssize_t bytes_written;

    while (write_out < n) {
        bytes_written = send(connect_fd, buffer + write_out, n - write_out, 0);
        if (bytes_written <= 0) {
            return -1;
        }
        //printf("Total no of Bytes written: %zd\n", bytes_written);
        write_out += bytes_written;
    }
    return write_out;
}

// Function to handle client read and write communication
void read_write(int connect_fd) {
    char buffer[MAX];
    ssize_t n;
    // Read a line from the client
    while (1) {
        n = readline(connect_fd, buffer, sizeof(buffer) - 1);
    
        buffer[n] = '\0';
        printf("\nData received from client: %s", buffer);
        if (writen(connect_fd, buffer, n) <0){
            perror("write error");
            break;
        }
    }

    close(connect_fd);
    exit(0);  // Exit the child process
}

int main(int argc, char *argv[]) {

    int port = atoi(argv[1]);
    int socket_fd, connect_fd;
    struct sockaddr_in server_address, client_address;
    socklen_t length = sizeof(client_address);
    pid_t child_id;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Socket Binding Failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(socket_fd, 5) < 0) {
        perror("Socket Listen failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d..\n", port);

    while (1) {
        connect_fd = accept(socket_fd, (struct sockaddr *)&client_address, &length);
        if (connect_fd < 0) {
            perror("Server accept failed");
            continue;
        }

        if ((child_id = fork()) == 0) {
            close(socket_fd);
            read_write(connect_fd);
            
        } else if (child_id > 0) {
            close(connect_fd);
            waitpid(-1, NULL, WNOHANG);  // wait all  child processes to finish
        } else {
            perror("Fork error");
        }
    }

    close(socket_fd);
    return 0;
}
