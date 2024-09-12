# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Executable names

SERVER = server
CLIENT = client
SERVER_LINK = echos
CLIENT_LINK = echo
# Source files
SERVER_SRC = server.c
CLIENT_SRC = client.c

# Object files
SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)

# Default target
all: $(SERVER) $(CLIENT) $(SERVER_LINK) $(CLIENT_LINK)

$(SERVER): $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_OBJ)
	@echo "Server executable built at $(SERVER)."

$(CLIENT): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_OBJ)
	@echo "Client executable built at $(CLIENT)."

$(SERVER_OBJ): $(SERVER_SRC)
	$(CC) $(CFLAGS) -c $(SERVER_SRC) -o $(SERVER_OBJ)
	@echo "Compiled $(SERVER_SRC) to $(SERVER_OBJ)."

$(CLIENT_OBJ): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -c $(CLIENT_SRC) -o $(CLIENT_OBJ)
	@echo "Compiled $(CLIENT_SRC) to $(CLIENT_OBJ)."
	
$(SERVER_LINK): $(SERVER)
	ln -sf $(SERVER) $(SERVER_LINK)

$(CLIENT_LINK): $(CLIENT)
	ln -sf $(CLIENT) $(CLIENT_LINK)

# Clean rule to remove built files:
clean:
	rm -f $(SERVER) $(CLIENT) $(SERVER_OBJ) $(CLIENT_OBJ) $(SERVER_LINK) $(CLIENT_LINK)
	@echo "Clean complete."

# Phony targets
.PHONY: all clean
