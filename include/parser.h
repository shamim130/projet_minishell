/**
 * @file parser.h
 * @author Shamim SEDGHI , Mathys
 * @brief function prototypes for command line parsing
 * @date december 2025
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef PARSER_H
#define PARSER_H

#include "typedef.h"

/**
 * @brief Parse an input command line into a sequence of commands and operators
 *
 * @param input The input command line
 * @param seq The sequence structure to fill
 * @return (int) 0 on success, -1 on failure with errno set
 */
int parse_command(char *input, sequence_t *seq);

#endif
