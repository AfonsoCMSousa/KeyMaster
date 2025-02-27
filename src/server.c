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
    char key[256];
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

    Request req;

    memset(&req, 0, sizeof(req));
    req.ID = 0;
    req.level = 0;
    req.type = 0;

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

        int n = recv(connfd, &req, sizeof(req), 0);
        if (n <= 0)
        {
            fprintf(stderr, "Client disconnected\n");
            isConnected = 0;
        }

        // debug
        printf("Received request\n");

        // TYPE == 0 is the equivelent of a GET
        if (req.type == 0)
        {
            // debug
            Request aux;
            printf("Received request type 0\n");

            char *filepath = create(char);
            filepath = size(filepath, 256);

            sprintf(filepath, "%s%d.bin", PASSWORDS, req.level);

            printf("Reading key from file: %s\n", filepath);

            int *buffer = NULL;

            try
            {
                buffer = create(int);
                buffer = size(buffer, 256);
            }
            catch (MEMORY_ALLOC_FAILURE)
            {
                fprintf(stderr, "Memory allocation failed\n");
                close(sockfd);
                return 1;
            }
            end_try;

            int i = readl(filepath, buffer, 256);
            if (i == -1)
            {

                aux.ID = req.ID;
                memset(&req, 0, sizeof(req));
                aux.type = req.type;
                aux.level = -1;
                send(connfd, &aux, sizeof(aux), 0);
            }

            // debug
            printf("Sending key: [%x]\n", buffer[0]);

            char sendKEY[256];

            decryptText(sendKEY, buffer);

            aux.ID = req.ID;
            for (size_t i = 0; i < 256; i++)
            {
                aux.key[i] = sendKEY[i];
            }
            aux.level = req.level;
            aux.type = req.type;
            send(connfd, &aux, sizeof(aux), 0);

            free(filepath);
        }
        else if (req.type == 1)
        {
            // debug
            printf("Received request type 1\n");

            char filepath[256];

            printf("Received key: %s\n", req.key);
            printf("Received level: %d\n", req.level);
            sprintf(filepath, "%s%d.bin", PASSWORDS, req.level);

            printf("Writing key to file: %s\n", filepath);
            int key2[256];

            emcryptText(key2, req.key);
            writel(filepath, key2, 256);
        }
        else if (req.type == 2)
        {
            // debug
            printf("Received request type 2\n");

            char filepath[256];

            printf("Received key: %s\n", req.key);
            printf("Received level: %d\n", req.level);
            sprintf(filepath, "%s%d.bin", PASSWORDS, req.level);

            printf("Deleting key from file: %s\n", filepath);

            if (remove(filepath) != 0)
            {
                fprintf(stderr, "Error deleting file\n");
            }
        }
        else
        {
            fprintf(stderr, "Invalid request type\n");
        }
    }

    // Close the socket
    close(connfd);
    close(sockfd);

    return 0;
}