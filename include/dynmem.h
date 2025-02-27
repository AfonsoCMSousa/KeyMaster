// Desenvolvido por: Afonso Sousa
// Versão: 1.1
// Data: 2024-11-05
// Descrição: Biblioteca para alocação de memória dinâmica em C e leitura de ficheiros (low-level)
#ifndef DYNMEM_H
#define DYNMEM_H

// Bibliotecas para o malloc e ficheiros
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include "dynmem.c"
#endif

/*

    Dynamic memory allocation for C

    Uso:
    #include "dynmem.h"

    int *vector = create(int);
    if (vector == NULL)
    {
        fprintf(stderr, "Falha na alocação de memoria para o vector\n");
        return 1;
    }

    vector = size(vector, 1);
    add(vector, 0, 10);

    printf("[%d]: %d\n", 0, vector[0]);

    free(vector);
*/

#define add(ptr, index, value) ptr[index] = value //@param ptr Pointer to add the value @param index Index of the value to add @param value Value to add
#define removePTR(ptr, index) ptr[index] = 0      //@param ptr Pointer to remove the value @param index Index of the value to remove

#ifdef __INTELLISENSE__
#define create(type) ((type *)malloc(sizeof(type)))                       //@param type Type of the vector @example int *vector = create(int);
#define size(ptr, size) ((void *)realloc((ptr), sizeof(*(ptr)) * (size))) //@param ptr Pointer to extend @param size New size of the pointer
#else
#define create(type) ((type *)malloc(sizeof(type)))                                //@param type Type of the vector @example int *vector = create(int);
#define size(ptr, size) ((__typeof__(ptr))realloc((ptr), sizeof(*(ptr)) * (size))) //@param ptr Pointer to extend @param size New size of the pointer
#endif

int readl(const char *filepath, void *buffer, size_t size);   //@param filepath Path to the file @param buffer Buffer to store the file @param size Size parameter specifies the number of bytes to read
void writel(const char *filepath, void *buffer, size_t size); //@param filepath Path to the file @param buffer Buffer to write to the file @param size Size parameter specifies the number of bytes to write

void emcryptText(int *output, char *password); //@param output Output buffer to store the encrypted text @param password Password to encrypt the text

void decryptText(char *output, int *password); //@param output Output buffer to store the decrypted text @param password Password to decrypt the text

#endif // DYNMEM_H