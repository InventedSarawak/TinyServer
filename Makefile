# Compiler and flags
CC = gcc
CFLAGS = -Wall

# Target executables
SERVER_TARGET = tcp_server
CLIENT_TARGET = tcp_client

# Source files
SERVER_SRC = Messaging/$(SERVER_TARGET).c
CLIENT_SRC = Messaging/$(CLIENT_TARGET).c

# Run 'make' to compile all targets
.PHONY: all clean run_server run_client

all: clean bin bin/$(SERVER_TARGET) bin/$(CLIENT_TARGET)

# Creates 'bin' directory if it doesn't exist
bin:
		mkdir -p bin

# Compiles tcp_server.c into the SERVER_TARGET executable
bin/$(SERVER_TARGET): $(SERVER_SRC) | bin
		$(CC) $(CFLAGS) -o $@ $<

# Compiles tcp_client.c into the CLIENT_TARGET executable
bin/$(CLIENT_TARGET): $(CLIENT_SRC) | bin
		$(CC) $(CFLAGS) -o $@ $<

# Clean up build files
clean:
		rm -rf bin/*.o
		rm -rf bin

# Run the tcp_server program with optional arguments
run_server: bin/$(SERVER_TARGET)
		./bin/$(SERVER_TARGET) $(ARGS)

# Run the tcp_client program with optional arguments
run_client: bin/$(CLIENT_TARGET)
		./bin/$(CLIENT_TARGET) $(ARGS)
