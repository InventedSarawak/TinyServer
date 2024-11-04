/*
    TinyServer message server: A simple, lightweight message server
    Usage: ./server port_number
    Example: ./server 80
*/

#include <stdio.h>              // Standard I/O library: printf(), perror()
#include <stdlib.h>             // Standard C library: exit(), atoi()
#include <string.h>             // String library: memset(), strcmp()
#include <unistd.h>             // UNIX standard library: read(), write(), close()
#include <sys/types.h>          // Primitive system data types: socklen_t
#include <sys/socket.h>         // Socket library: socket(), bind(), listen(), accept()
#include <netinet/in.h>         // Internet address library: struct sockaddr_in
#include <netdb.h>              // Network database library: gethostbyname()
#include <arpa/inet.h>          // For inet_addr
#include <pthread.h>            // POSIX threads library
#include <stdint.h>             // Standard integer types
#include <semaphore.h>          // POSIX semaphore library
#include <signal.h>             // Signal handling library
#include <time.h>               // Time library

#define MAX_BUFFER_SIZE 1024
#define SA struct sockaddr
#define SA_IN struct sockaddr_in
#define MAX_CLIENTS 10

int client_sockets[MAX_CLIENTS];                // Array to hold client sockets
int client_count = 0;                           // Number of active clients
pthread_mutex_t clients_mutex;                  // Mutex for thread-safe access to client list
int sockfd, newsockfd = -1;                     // Server and client socket file descriptors
time_t start_time, end_time;                    // Will help to find the idle time of the server

void error(const char *msg);
int create_socket();
SA_IN setup_server_address(int portno);
void bind_socket(int sockfd, SA_IN serv_addr);
void listen_for_connections(int sockfd);
int accept_connection(int sockfd, SA_IN *cli_addr);
void *receive_message(void *newsockfd_ptr);
void close_sockets(int newsockfd, int sockfd);
void sig_int_handler(int sig);  // Function prototype first

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Port number not specified\n");
        return 1;
    }
    pthread_mutex_init(&clients_mutex, NULL);
    int portno = atoi(argv[1]); // Get the port number
    SA_IN serv_addr, cli_addr;

    // Set up signal handler
    struct sigaction sa;
    sa.sa_handler = sig_int_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    // Create a socket
    sockfd = create_socket();

    // Set up the server address
    serv_addr = setup_server_address(portno);

    // Bind the socket
    bind_socket(sockfd, serv_addr);

    // Listen for connections
    listen_for_connections(sockfd);

    // Accept connections in a loop
    while (1) {
        // Accept a new connection
        newsockfd = accept_connection(sockfd, &cli_addr);
        
        // Create a thread for each client
        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, receive_message, (void *)(intptr_t)newsockfd) != 0) {
            perror("ERROR creating thread");
            close(newsockfd);
        }

        // Detach the thread so that its resources are released when it finishes
        pthread_detach(client_thread);
    }
    return 0;
}

/**
 * Prints an error message and exits the program with status code 1.
 *
 * @param msg The error message to be printed.
 */
void error(const char *msg) {
    perror(msg);
    exit(1);
}

/**
 * Creates a new socket for communication with the client.
 *
 * @return The file descriptor of the socket.
 */
int create_socket() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) error("ERROR opening socket"); 
    else printf("Socket created successfully\n");
    return sockfd;
}

/**
 * Sets up a server address structure for binding a socket to the specified port number.
 *
 * @param portno The port number to bind the socket to.
 *
 * @return The server address structure.
 */
SA_IN setup_server_address(int portno) {
    SA_IN serv_addr;
    bzero((char *)&serv_addr, sizeof(serv_addr)); // Zero out the structure
    serv_addr.sin_family = AF_INET;                // IPv4 family type
    serv_addr.sin_addr.s_addr = INADDR_ANY;       // Bind to all interfaces
    serv_addr.sin_port = htons(portno);            // Port number
    return serv_addr;
}

/**
 * Binds the socket to the server address.
 *
 * @param sockfd The file descriptor of the socket.
 * @param serv_addr The server address structure.
 */
void bind_socket(int sockfd, SA_IN serv_addr) {
    if (bind(sockfd, (SA *)&serv_addr, sizeof(serv_addr)) < 0) error("ERROR on binding"); 
    else printf("Binding successful\n");
}

/**
 * Listens for incoming connections on the specified socket.
 *
 * @param sockfd The file descriptor of the socket on which to listen.
 */
void listen_for_connections(int sockfd) {
    listen(sockfd, MAX_CLIENTS); // Maximum number of queued connections
    printf("Listening for connections\n" "Waiting for incoming connections..." "\n");
}

/**
 * Accepts an incoming connection on the specified socket.
 *
 * @param sockfd The file descriptor of the socket to accept the connection on.
 * @param cli_addr The server address structure to store the client's address in.
 *
 * @return The file descriptor of the new socket associated with the client.
 */
int accept_connection(int sockfd, SA_IN *cli_addr) {
    socklen_t cli_len = sizeof(*cli_addr);
    int newsockfd = accept(sockfd, (SA *)cli_addr, &cli_len);
    if (newsockfd < 0) error("ERROR on accept"); 
    else {
        printf("Accepted connection\n");
        // Add the new client to the client_sockets array
        if (client_count < MAX_CLIENTS) client_sockets[client_count++] = newsockfd;
        else {
            printf("Max clients reached, closing new connection.\n");
            close(newsockfd); // Close if max clients reached
        }
        pthread_mutex_unlock(&clients_mutex); // Unlock the mutex
    }
    return newsockfd;
}

/**
 * Reads a message from the user and sends it to the client over the specified socket.
 *
 * The user is prompted to enter a message, which is then sent to the client using
 * the write() system call. If the write fails, an error is printed to the console.
 *
 * @param sockfd The file descriptor of the socket to write to.
 */
void send_message(int sockfd) {
    char message[MAX_BUFFER_SIZE]; // Buffer to hold the message
    printf("Sent: ");
    if (fgets(message, sizeof(message), stdin) == NULL) error("ERROR reading from stdin");
    // Remove the newline character from fgets, if it exists
    message[strcspn(message, "\n")] = 0;
    // Send the message
    if (write(sockfd, message, strlen(message)) < 0) error("ERROR writing to socket"); 
}

/**
 * Handles an incoming connection from a client in a separate thread.
 *
 * This function is called by the main server loop when a client connects.
 * It continuously reads incoming messages from the client and prints them to the console.
 * When the client sends a message with one of the exit commands ("exit", "quit", "close", "end", "bye"),
 * the function breaks out of the loop and stops receiving messages.
 * The function also sends a response back to the client after each incoming message.
 *
 * @param newsockfd_ptr A pointer to the new socket created for the accepted connection.
 */
void *receive_message(void *newsockfd_ptr) {
    int newsockfd = (intptr_t)newsockfd_ptr; // Convert back to socket descriptor
    char buffer[MAX_BUFFER_SIZE];
    int numbytes;

    while (1) {
        bzero(buffer, MAX_BUFFER_SIZE); // Clear the buffer
        numbytes = recv(newsockfd, buffer, MAX_BUFFER_SIZE, 0);
        
        if (numbytes < 0) {
            error("ERROR reading from socket");
            break;
        } else if (numbytes == 0) {
            printf("Client disconnected\n");
            break; // Client disconnected
        }

        // Null-terminate the received data
        buffer[numbytes] = '\0';

        // Check for exit commands
        if (strcmp(buffer, "exit") == 0 || strcmp(buffer, "quit") == 0 ||
            strcmp(buffer, "close") == 0 || strcmp(buffer, "end") == 0 ||
            strcmp(buffer, "bye") == 0) {
            break;
        }

        // Print the received message
        printf("Received: %s\n", buffer);

        // Broadcast the received message to all other clients
        pthread_mutex_lock(&clients_mutex); // Lock the mutex for thread-safe access
        for (int i = 0; i < client_count; i++) {
            if (client_sockets[i] != newsockfd) write(client_sockets[i], buffer, numbytes);
        }
        pthread_mutex_unlock(&clients_mutex); // Unlock the mutex
    }

    // Remove client from client_sockets array
    pthread_mutex_lock(&clients_mutex); // Lock the mutex
    for (int i = 0; i < client_count; i++) {
        if (client_sockets[i] == newsockfd) {
            client_sockets[i] = client_sockets[--client_count]; // Remove client
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex); // Unlock the mutex
    close(newsockfd);
    return NULL;
}

/**
 * Closes both the new socket (created by accept()) and the original socket
 * used for listening. This is necessary to free up system resources.
 *
 * @param newsockfd The file descriptor of the new socket created by accept()
 * @param sockfd The file descriptor of the original socket used for listening
 */
void close_sockets(int newsockfd, int sockfd) {
    close(newsockfd);
    close(sockfd);
    printf("\n" "Sockets closed" "\n");
}

/**
 * Handles the SIGINT signal (Ctrl+C) by closing all connections and exiting the program.
 *
 * This function is called when the user presses Ctrl+C while the server is running.
 * It closes all the sockets and prints a message to the console before exiting the program.
 *
 * @param sig The signal number (SIGINT) that was raised
 */
void sig_int_handler(int sig) {
    printf("\b\b  \b\b");
    int i = 0;
    signal(sig, SIG_IGN);
    printf("Closing the server and all connections" "\n");
    while (i < 3) {
        usleep(400000);
        printf(".");
        fflush(stdout);
        i++;
        if (i == 3) usleep(300000);
    }
    close_sockets(sockfd, sockfd);
    printf("Server closed");
    exit(0);
}