/*
    TinyServer message client: A simple, lightweight messaging client
    Usage: ./client hostname port_number
    Example: ./client localhost 80
*/

#include <stdio.h>              // Standard I/O library: printf(), perror()
#include <stdlib.h>             // Standard C library: exit(), atoi()
#include <string.h>             // String library: memset(), strlen(), strcmp()
#include <unistd.h>             // UNIX standard library: read(), write(), close()
#include <sys/types.h>          // Primitive system data types: socklen_t
#include <sys/socket.h>         // Socket library: socket(), bind(), listen(), accept()
#include <netinet/in.h>         // Internet address library: struct sockaddr_in
#include <netdb.h>              // Network database library: gethostbyname()
#include <arpa/inet.h>          // For inet_addr

#define MAX_BUFFER_SIZE 1024
#define SA struct sockaddr
#define SA_IN struct sockaddr_in

void error(const char *msg);
int create_socket();
struct in_addr get_hostname(const char *ip);
SA_IN setup_socket(struct in_addr ip, int portno);
void connect_socket(int sockfd, SA_IN serv_addr);
void send_message(int sockfd);
void receive_message(int sockfd);
void close_socket(int sockfd);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        exit(1);
    }

    int portno = atoi(argv[2]), sockfd; // Get the port number
    struct in_addr ip;                  // Server IP
    SA_IN serv_addr;                   // Server address

    // Get the server hostname
    ip = get_hostname(argv[1]);

    // Create a socket
    sockfd = create_socket();

    // Set the server address
    serv_addr = setup_socket(ip, portno);

    // Connect the socket
    connect_socket(sockfd, serv_addr);

    // Send some data
    send_message(sockfd);

    // Receive a reply
    receive_message(sockfd);

    // Close the socket
    close_socket(sockfd);
    
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
 * Creates a new socket for communication with the server.
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
 * Converts a hostname to its corresponding IP address.
 *
 * @param ip The hostname to be converted.
 *
 * @return The struct in_addr containing the IP address of the hostname.
 */
struct in_addr get_hostname(const char *ip) {
    struct hostent *server = gethostbyname(ip);
    if (!server) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    } 
    return *(struct in_addr *)server->h_addr_list[0]; // Return the first address
}

/**
 * Sets up a server address structure for connecting to a server.
 *
 * @param ip The IP address of the server.
 * @param portno The port number to connect to.
 *
 * @return The server address structure.
 */
SA_IN setup_socket(struct in_addr ip, int portno) {
    SA_IN serv_addr;
    bzero((char *)&serv_addr, sizeof(serv_addr)); // Zero out the structure
    serv_addr.sin_family = AF_INET;                // IPv4 family type
    serv_addr.sin_addr = ip;                       // Set the server IP
    serv_addr.sin_port = htons(portno);            // Set the port number
    return serv_addr;
}

/**
 * Connects the socket to the specified server address.
 *
 * @param sockfd The file descriptor of the socket to be connected.
 * @param serv_addr The server address structure to connect to.
 */
void connect_socket(int sockfd, SA_IN serv_addr) {
    if (connect(sockfd, (SA *)&serv_addr, sizeof(serv_addr)) < 0) error("ERROR connecting"); 
    else printf("Connected successfully\n");
}


/**
 * Reads a message from the user and sends it to the server over the specified socket.
 *
 * The user is prompted to enter a message, which is then sent to the server using
 * the write() system call. If the write fails, an error is printed to the console.
 *
 * @param sockfd The file descriptor of the socket to write to.
 */
void send_message(int sockfd) {
    char message[MAX_BUFFER_SIZE]; // Buffer to hold the message
    printf("Sent: ");
    if (fgets(message, sizeof(message), stdin) == NULL) error("ERROR reading from stdin");
    // Remove the newline   character from fgets, if it exists
    message[strcspn(message, "\n")] = 0;
    // Send the message
    if (write(sockfd, message, strlen(message)) < 0) error("ERROR writing to socket"); 
}
 


/**
 * Receives and handles messages from the server through the specified socket.
 *
 * This function continuously reads incoming messages from the server and
 * prints them to the client's console. When the server sends a message
 * with one of the exit commands ("exit", "quit", "close", "end", "bye"),
 * the function breaks out of the loop and stops receiving messages.
 * 
 * After printing the received message, the function prompts the client
 * user to enter a response, which is then sent back to the server.
 *
 * @param sockfd The file descriptor of the socket connected to the server.
 */
void receive_message(int sockfd) {
    char buffer[MAX_BUFFER_SIZE];
    int len;

    while (1) {
        bzero(buffer, MAX_BUFFER_SIZE); // Clear the buffer
        len = read(sockfd, buffer, MAX_BUFFER_SIZE);
        
        if (len < 0) {
            error("ERROR reading from socket");
        } else if (len == 0) {
            // Connection has been closed
            printf("Server closed the connection.\n");
            break;
        }

        // Null-terminate the received data
        buffer[len] = '\0'; 
        printf("Server: %s\n", buffer);

        // Check for exit commands from the server
        if (strcmp(buffer, "exit") == 0 || strcmp(buffer, "quit") == 0 ||
            strcmp(buffer, "close") == 0 || strcmp(buffer, "end") == 0 ||
            strcmp(buffer, "bye") == 0) {
            printf("Received exit command from server. Exiting...\n");
            break;  // Exit the loop
        }

        // Prompt for a new message to send to the server
        send_message(sockfd);
    }
}



/**
 * Closes a socket to free up system resources.
 *
 * This function is used to close the socket when it is no longer needed.
 *
 * @param sockfd The file descriptor of the socket to be closed.
 */
void close_socket(int sockfd) {
    printf("Closing socket\n");
    close(sockfd);
}
