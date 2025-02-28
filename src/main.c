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
    char key[256];
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
            if (prompPassword(password, "Please enter your unique password: ") == -1)
            {
                return 1;
            }
            printf("\n");

            int *buffer = create(int);
            buffer = size(buffer, 256);

            encryptText(buffer, password);
            writel(PASS_FILE, buffer, 256);

            free(buffer);
        }
        else
        {
            return 0;
        }
    }
    printf("KeyMaster\nVersion: 0.3.1\n\n");

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
            prompPassword(askedPass, "Please enter your unique password: ");
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
        Request req;

        // send request with type 3 to get IDs for all passwords
        // level == how many keys are being sent
        // key == the list of levels that have keys

        req.type = 3;
        memset(&req.key, 0, sizeof(req.key));
        req.ID = 1;
        req.level = 0;

        send(sockfd, &req, sizeof(req), 0);

        recv(sockfd, &req, sizeof(req), 0);
        if (req.level == -1)
        {
            printf("No passwords found\n");
        }
        else
        {
            // Debug the response
            for (size_t i = 0; i < req.level; i++)
            {
                Request req1;
                req1.type = 0;
                memset(&req1.key, 0, sizeof(req1.key));
                req1.ID = 1;
                req1.level = req.key[i];

                send(sockfd, &req1, sizeof(req1), 0);

                recv(sockfd, &req1, sizeof(req1), 0);
                if (req1.level != -1 && strlen(req1.key) > 0)
                {
                    printf("Level: %d\tKey: %s\n", req1.level, req1.key);
                }
            }
        }

        printf("<------->\n\n");
        choice = prompNormalRequest(">> ");
        if (strcmp(choice, "exit") == 0 || strcmp(choice, "quit") == 0 || strcmp(choice, "q") == 0)
        {
            break;
        }
        else if (strcmp(choice, "addpass") == 0)
        {
            char *aux = create(char);
            aux = size(aux, 256);
            prompPassword(aux, "Please enter your new password: ");
            printf("\n");
            char *aux2 = create(char);
            aux2 = size(aux2, 256);
            prompPassword(aux2, "Confirm new password: ");
            printf("\n");

            if (strcmp(aux, aux2) != 0)
            {
                printf("Passwords do not match\nStopping...\n");
                free(aux);
                free(aux2);
                continue;
            }

            printf("Please enter the level of security for the password (1-255) (higher = more complexity): ");
            int level;
            scanf("%d", &level);
            clear_input_buffer();

            if (level < 0 && level > 255)
            {
                printf("Invalid level of security\nStopping...\n");
                free(aux);
                free(aux2);
                continue;
            }

            // send request with TYPE = 1
            req.type = 1;
            for (size_t i = 0; i < 256; i++)
            {
                req.key[i] = aux[i];
            }
            req.key[sizeof(req.key) - 1] = '\0'; // Ensure null-termination
            req.ID = 1;
            req.level = level;

            send(sockfd, &req, sizeof(req), 0);
            free(aux);
            free(aux2);
        }
        else if (strcmp(choice, "delpass") == 0)
        {
            printf("Please enter the level of security for the password to delete (1-255): ");
            int level;
            scanf("%d", &level);
            clear_input_buffer();

            if (level < 0 && level > 255)
            {
                printf("Invalid level of security\nStopping...\n");
                continue;
            }
            // send request with TYPE = 2
            req.type = 2;
            memset(&req.key, 0, sizeof(req.key));
            req.ID = 1;
            req.level = level;

            send(sockfd, &req, sizeof(req), 0);
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