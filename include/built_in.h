/**
 * @file built_in.h
 * @author Shamim SEDGHI , Mathys
 * @brief types and function prototypes for built-in shell commands
 * @date december 2025
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef BUILTIN_H
#define BUILTIN_H

#include "typedef.h"

/**
 * @brief verification if a command is a built-in command
 *
 * @param cmd The command to check
 * @return (int) the result (1 if it is a built-in, 0 otherwise)
 */
int is_builtin(command_t *cmd);

/**
 * @brief Execute a built-in command
 *
 * @param cmd The command to execute
 * @return (int) The exit status of the built-in command
 */
int execute_builtin(command_t *cmd);

int builtin_alias(command_t *cmd);


#endif
