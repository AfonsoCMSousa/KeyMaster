#include <termios.h>
#include "herror.h"
#include "dynmem.h"

int prompPassword(char *password, const char *msg)
{
    struct termios oldt, newt;

    printf(msg);
    try
    {
        // Disable echo
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        do
        {
            scanf("%s", password);
        } while (password[0] == '\0');

        // Restore echo
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
    catch (MEMORY_ALLOC_FAILURE)
    {
        printf("Memory allocation failure\n");
        return 1;
    }
    end_try;

    printf("\n");

    return 0;
}

char *prompNormalRequest(char *message)
{
    char *temp = create(char);

    temp = size(temp, 256);

    printf("%s", message);
    scanf("%s", temp);

    return temp;
}