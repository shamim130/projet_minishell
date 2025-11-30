/**
 * @file shell.c
 * @brief Implémentation des fonctions principales du shell
 */

#include <stdio.h>
#include <string.h>
#include "../include/shell.h"
#include "../include/error.h"

void print_prompt(void)
{
    printf("Prompt> ");
    fflush(stdout);
}

int read_line(char *buffer, int size)
{
    if (!fgets(buffer, size, stdin)) {
        if (ferror(stdin))
            print_sys_error("fgets");
        return 0;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    return 1;
}
