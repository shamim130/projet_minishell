/**
 * @file shell.h
 * @author Shamim SEDGHI , Mathys
 * @brief  The main shell function prototypes
 * @date december 2025
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef SHELL_H
#define SHELL_H
#include "typedef.h"

#define MAX_LINE 1024
#define MAX_ARGS 64

/**
 * @brief Display the shell prompt
 *
 */
void print_prompt(void);

/**
 * @brief Read a line of input from the user
 *
 * @param buffer The buffer to store the input line
 * @param size The size of the buffer
 * @return (int) 1 on success, 0 on EOF or error
 */
int read_line(char *buffer, int size);

#endif /* SHELL_H */