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
#define PASSWORDS "./files/USER_PASSWORDS_L"

typedef struct Request
{
    unsigned char type;
    char *key;
    int ID;
    int level;
} Request;

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
    u_int32_t len = sizeof(cli);

    char *buffer = create(char);
    buffer = size(buffer, 256);

    u_int8_t isConnected = 0;
    int connfd;

    // Listen to the client requests
    while (1)
    {
        // Receive the request

        if (!isConnected)
        {
            connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
            if (connfd < 0)
            {
                fprintf(stderr, "Server accept failed\n");
                close(sockfd);
            }
        }

        isConnected = 1;

        printf("Client connected\n");

        Request req;
        int n = recv(connfd, &req, sizeof(Request), 0);
        if (n <= 0)
        {
            fprintf(stderr, "Client disconnected\n");
            isConnected = 0;
        }

        // TYPE == 0 is the equivelent of a GET
        if (req.type == 0)
        {
            char *filepath = PASSWORDS;
            sprintf(filepath, "%s%d.bin", filepath, req.level);
            if (readl(filepath, buffer, 256) == -1)
            {
                Request aux;
                aux.ID = req.ID;
                aux.key = NULL;
                aux.type = req.type;
                aux.level = -1;
                send();
            }
        }
    }

    // Close the socket
    close(connfd);
    close(sockfd);

    return 0;
}