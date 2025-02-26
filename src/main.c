#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <curl/curl.h> // Include libcurl header

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

// Function to perform a GET request
void perform_get_request(const char *url)
{
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "GET request failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

// Function to perform a POST request
void perform_post_request(const char *url, const char *post_fields)
{
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "POST request failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
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

    // Example usage of GET and POST requests
    const char *get_url = "http://148.71.10.137:51820/";
    const char *post_fields = "field1=value1&field2=value2";

    // Menu loop
    while (1)
    {

        // Connect to the server and get the list of passwords tro HTTP GET
        free(choice);
    }
    free(password);
    return 0;
}