#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "herror.h"
#include "dynmem.h"
#include "ui/ui.h"

#define SERVER_IP "192.168.1.120"
#define PASSWORDS "./files/KEYS.bin"

int main(void)
{
    // Connect and bind to a port (8080) on the server
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        fprintf(stderr, "Socket creation failed\n");
        return 1;
    }

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        fprintf(stderr, "Socket bind failed\n");
        close(sockfd);
        return 1;
    }

    if (listen(sockfd, 5) != 0)
    {
        fprintf(stderr, "Listen failed\n");
        close(sockfd);
        return 1;
    }

    printf("Server listening on %s:8080\n", SERVER_IP);

    struct sockaddr_in cli;
    int len = sizeof(cli);

    int connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
    if (connfd < 0)
    {
        fprintf(stderr, "Server accept failed\n");
        close(sockfd);
        return 1;
    }

    printf("Client connected\n");

    // Listen to the client requests
    while (1)
    {
        char buffer[1024] = {0};
        int bytes_read = read(connfd, buffer, sizeof(buffer));
        if (bytes_read <= 0)
        {
            fprintf(stderr, "Client disconnected\n");
            break;
        }

        printf("Received request: %s\n", buffer);

        // Respond with the list of passwords
        FILE *file = fopen(PASSWORDS, "r");
        if (file == NULL)
        {
            fprintf(stderr, "Could not open password file\n");
            break;
        }

        char passwords[1024] = {0};
        fread(passwords, sizeof(char), sizeof(passwords), file);
        fclose(file);

        write(connfd, passwords, strlen(passwords));
    }

    // Close the socket
    close(connfd);
    close(sockfd);

    return 0;
}