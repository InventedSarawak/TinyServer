#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 80
#define MAX_BUFFER_SIZE 1024
#define SA struct sockaddr

void error(char *msg) {
    perror(msg);
    exit(1);
}


// TODO: @InventedSarawak
// Create a TCP Server with a socket to listen for incoming connections
errno = 0;
int main(int argc, char *argv[]) {
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);   
    // AF_INET -> IPv4, SOCK_STREAM -> TCP
    // Other arguments : AF_INET6 -> IPv6, SOCK_DGRAM -> UDP, SOCK_RAW -> Raw IP packets
    if (sockfd == -1) {
        printf("The Value of the error %d\n", errno);
        return 1;
    } else printf("Socket created successfully\n");

   SA addr; 

    // int bind = bind(sockfd, (SA *)NULL, 0);
}
