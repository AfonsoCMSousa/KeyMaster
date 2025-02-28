#include "dynmem.h"

#include <sys/stat.h>
// Reads a file and stores it in a buffer (automatically opens, reads and closes the file)
//@warning "readl" only reads the first line! Possible side effects may include slow performance if done multiple times
//@param filepath Path to the file
//@param buffer Buffer to store the file
//@param size Size parameter specifies the number of bytes to read
int readl(const char *filepath, void *buffer, size_t size)
{
    int fd;
#ifdef _WIN32
    fd = _open(filepath, _O_RDONLY | _O_BINARY); // Open file in binary mode
#else
    fd = open(filepath, O_RDONLY); // Files are opened in binary mode by default on Unix-like systems
#endif
    if (fd == -1)
    {
        return fd;
    }

    size_t bytesRead = read(fd, buffer, size);
    if (bytesRead == -1)
    {
        return bytesRead;
    }

#ifdef _WIN32
    _close(fd);
#else
    close(fd);
#endif

    return 1;
}

// Writes a buffer to a file (automatically opens, writes and closes the file)
//@warning "writel" only writes to the first line! Possible side effects may include slow performance if done multiple times
//@param filepath Path to the file
//@param buffer Buffer to write to the file
//@param size Size parameter specifies the number of bytes to write
void writel(const char *filepath, void *buffer, size_t size)
{
    int fd;
#ifdef _WIN32
    fd = _open(filepath, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IREAD | _S_IWRITE); // Open file in binary mode
#else
    fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); // Files are opened in binary mode by default on Unix-like systems
#endif
    if (fd == -1)
    {
        fprintf(stderr, "Failed to open file %s\n", filepath);
        return;
    }

    size_t bytesWritten = write(fd, buffer, size);
    if (bytesWritten == -1)
    {
        fprintf(stderr, "Failed to write to file %s\n", filepath);
    }

#ifdef _WIN32
    _close(fd);
#else
    close(fd);
#endif
}

void encryptText(int *output, char *password)
{
    for (int i = 0; i < 256; i++)
    {
        output[i] = (int)password[i];
        output[i] ^= 0xFF; // Invert bitwise all characters in the password
        output[i] = output[i] * 2;
    }
}

void decryptText(char *output, int *password)
{
    for (int i = 0; i < 256; i++)
    {
        int temp = password[i] / 2;
        output[i] = (char)(temp ^ 0xFF); // Reverse the XOR and division operations
    }
}
