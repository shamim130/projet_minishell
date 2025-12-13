/**
 * @file executer.h
 * @author Shamim SEDGHI , Mathys
 * @brief Function prototypes for command execution
 * @date 2025-12-13
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef EXECUTER_H
#define EXECUTER_H

#include "typedef.h"

/**
 * @brief Execute a sequence of commands with logical operators and pipes
 *
 * @param seq The sequence of commands to execute
 * @return (int) The exit status of the last executed command
 */
int execute_command(sequence_t *seq);

#endif
