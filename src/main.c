#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

#include "herror.h"
#include "dynmem.h"
#include "ui/ui.h"

#define PASS_FILE "./files/KEYS.bin"

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
            writel(PASS_FILE, buffer, 256 * sizeof(int));

            free(buffer);
        }
        else
        {
            return 0;
        }
    }
    printf("KeyMaster\nVersion: 0.1\n\n");

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
    free(password);

    // Menu loop
    while (1)
    {
        printf("1. Add a new password\n2. Show all passwords\n3. Exit\n");
        char *choice = prompNormalRequest("Enter your choice: ");
        if (strcmp(choice, "1") == 0)
        {
            printf("Adding a new password\n");
        }
        else if (strcmp(choice, "2") == 0)
        {
            printf("Showing all passwords\n");
        }
        else if (strcmp(choice, "3") == 0)
        {
            printf("Exiting...\n");
            break;
        }
        else
        {
            printf("Invalid choice\n");
        }
        free(choice);
    }

    return 0;
}