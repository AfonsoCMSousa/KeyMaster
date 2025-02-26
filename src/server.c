#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "herror.h"
#include "dynmem.h"
#include "ui/ui.h"

#define SERVER_IP "XXX.XXX.XXX.XXX"

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
}