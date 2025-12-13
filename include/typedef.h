/**
 * @file typedef.h
 * @author Shamim SEDGHI , Mathys
 * @brief typedefs used in the shell project
 * @date december 2025
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TYPEDEF_H
#define TYPEDEF_H

#define MAX_LINE 1024
#define MAX_ARGS 64
#define MAX_COMMANDS 3
#define HISTORY_FILE ".my_shell_history"

/**
 * @brief Structure representing a command with its arguments and redirections
 *
 */
typedef struct s_command
{
    char *argv[MAX_ARGS];
    int argc;

    char *input_file;    // input redirection file
    char *output_file;   // output redirection file
    int append_output;   // flag for appending to output file
    int background;      // flag for background execution
    char *heredoc_delim; // heredoc delimiter
} command_t;

/**
 * @brief Enumeration of logical operators between commands
 *
 */
typedef enum e_logical_operator
{
    LOGICAL_NONE,
    LOGICAL_AND,
    LOGICAL_OR,
    LOGICAL_PIPE
} logical_operator_t;

/**
 * @brief Structure representing a sequence of commands and their connecting operators
 *
 */
typedef struct s_sequence
{
    command_t commands[MAX_COMMANDS];
    logical_operator_t operators[MAX_COMMANDS - 1];
    int count;
} sequence_t;

#endif
