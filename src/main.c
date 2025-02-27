#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "herror.h"
#include "dynmem.h"
#include "ui/ui.h"

#define PASS_FILE "./files/KEYS.bin"
#define SERVER_IP "192.168.1.120"
#define SERRER_PORT 8080

typedef struct Request
{
    unsigned char type;
    char *key;
    int ID;
    int level;
} Request;

void clear_input_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int main(void)
{
    char *password = create(char);
    password = size(password, 256);

    if (password == NULL)
    {
        fprintf(stderr, "Memory allocation failure\n");
        return 1;
    }

    int fP = readl(PASS_FILE, password, strlen(password));
    if (fP == -1)
    {
        printf("Couldnt find the password file\nWould you like to create a new one? (y/n): ");
        char c;
        scanf("%c", &c);
        clear_input_buffer();
        if (c == 'y' || c == 'Y')
        {
            if (prompPassword(password) == -1)
            {
                return 1;
            }
            printf("\n");

            int *buffer = create(int);
            buffer = size(buffer, 256);

            emcryptText(buffer, password);
            writel(PASS_FILE, buffer, 256);

            free(buffer);
        }
        else
        {
            return 0;
        }
    }
    printf("KeyMaster\nVersion: 0.2\n\n");

    try
    {
        int *key = create(int);
        key = size(key, 256);
        if (key == NULL)
        {
            throw(MEMORY_ALLOC_FAILURE);
        }

        readl(PASS_FILE, key, 256) == -1 ? throw(FILE_NOT_FOUND) : 0;
        decryptText(password, key);

        char *askedPass = create(char);
        askedPass = size(askedPass, 256);
        if (askedPass == NULL)
        {
            throw(MEMORY_ALLOC_FAILURE);
        }

        for (size_t i = 0; i < 4; i++)
        {
            prompPassword(askedPass);
            if (strcmp(password, askedPass) != 0)
            {
                printf("Invalid password, tries left: %zu\n", 2 - i);
            }
            else
            {
                break;
            }

            if (i == 2)
            {
                printf("Too many tries, exiting...\n");
                return 1;
            }
        }

        printf("Welcome to KeyMaster\n");
        free(askedPass);
        free(key);
    }
    catch (MEMORY_ALLOC_FAILURE)
    {
        fprintf(stderr, "Memory allocation failure\n");
        return 1;
    }
    catch (FILE_NOT_FOUND)
    {
        fprintf(stderr, "Couldnt find the password file or its empty\n");
        return 1;
    }
    end_try;

    char *choice = NULL;

    try
    {
        choice = create(char);
        choice = size(choice, 256);

        if (choice == NULL)
        {
            throw(MEMORY_ALLOC_FAILURE);
        }
    }
    catch (MEMORY_ALLOC_FAILURE)
    {
        fprintf(stderr, "Memory allocation failure\n");
        return 1;
    }
    end_try;

    // Connect to the server
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        fprintf(stderr, "Socket creation failed\n");
        return 1;
    }

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERRER_PORT);
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        fprintf(stderr, "Connection to the server failed\n");
        return 1;
    }

    // Menu loop
    while (1)
    {
        printf("\nList of existing passwords:\n<------->\n");

        // Send a request with type 0 (REQUEST)
        Request req;
        req.type = 0;
        req.key = NULL;
        req.ID = 0;

        send(sockfd, &req, sizeof(Request), 0);

        // Receive response from the server
        recv(sockfd, &req, sizeof(req), 0);
        if (req.level == -1)
        {
            printf("No existing passwords, use \"addpass\" command to add a password to the list.\n");
        }

        printf("<------->\n\n");
        choice = prompNormalRequest(">> ");
        if (strcmp(choice, "exit") == 0 || strcmp(choice, "quit") == 0 || strcmp(choice, "q") == 0)
        {
            break;
        }
        else if (strcmp("choise", "addpass"))
        {
            printf("adding new password to the server.");
        }
        else
        {
            printf("Invalid command (%s)\n\"Q\" or \"quit\" or \"exit\" to close the program\n", choice);
        }
    }

    // Close the socket
    close(sockfd);

    free(choice);
    free(password);
    return 0;
}