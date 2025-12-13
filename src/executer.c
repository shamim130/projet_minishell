/**
 * @file executer.c
 * @author Shamim SEDGHI , Mathys
 * @brief execution of commands and handling of processes
 * @date december 2025
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include "../include/typedef.h"
#include "../include/error.h"
#include "../include/executer.h"
#include "../include/built_in.h"

/**
 * @brief Execute a single command, handling redirections and heredocs
 *
 * @param cmd The command to execute
 * @return (int) The exit status of the command
 */
static int execute_one_command(command_t *cmd)
{
    pid_t pid;
    int status;
    int pipefd[2] = {-1, -1};

    if (!cmd || cmd->argc == 0 || cmd->argv[0] == NULL)
        return 0;

    // if heredoc is specified, create a pipe
    if (cmd->heredoc_delim)
    {
        if (pipe(pipefd) == -1)
        {
            print_sys_error("pipe");
            return errno;
        }
    }

    pid = fork();
    if (pid == -1)
    {
        print_sys_error("fork");
        if (pipefd[0] != -1)
        {
            close(pipefd[0]);
            close(pipefd[1]);
        }
        return errno;
    }

    // child
    if (pid == 0)
    {

        if (cmd->heredoc_delim)
        {
            // read from the lecture end of the pipe
            close(pipefd[1]);
            if (dup2(pipefd[0], STDIN_FILENO) == -1)
            {
                print_sys_error("dup2 heredoc");
                close(pipefd[0]);
                exit(EXIT_FAILURE);
            }
            close(pipefd[0]);
        }
        else if (cmd->input_file)
        {
            int fd_in = open(cmd->input_file, O_RDONLY);
            if (fd_in == -1)
            {
                print_sys_error("open input");
                exit(EXIT_FAILURE);
            }
            if (dup2(fd_in, STDIN_FILENO) == -1)
            {
                print_sys_error("dup2 input");
                close(fd_in);
                exit(EXIT_FAILURE);
            }
            close(fd_in);
        }

        // redirection of the output if specified
        if (cmd->output_file)
        {
            int flags = O_WRONLY | O_CREAT |
                        (cmd->append_output ? O_APPEND : O_TRUNC);
            int fd_out = open(cmd->output_file, flags, 0644);
            if (fd_out == -1)
            {
                print_sys_error("open output");
                exit(EXIT_FAILURE);
            }
            if (dup2(fd_out, STDOUT_FILENO) == -1)
            {
                print_sys_error("dup2 output");
                close(fd_out);
                exit(EXIT_FAILURE);
            }
            close(fd_out);
        }

        execvp(cmd->argv[0], cmd->argv);
        print_sys_error("execvp");
        exit(EXIT_FAILURE);
    }

    // parent
    if (cmd->heredoc_delim)
    {
        char buffer[1024];

        close(pipefd[0]); // using just the write end of the pipe

        while (1)
        {
            if (!fgets(buffer, sizeof(buffer), stdin))
                break;
            buffer[strcspn(buffer, "\n")] = '\0';

            if (strcmp(buffer, cmd->heredoc_delim) == 0)
                break;

            size_t len = strlen(buffer);
            if (write(pipefd[1], buffer, len) == -1)
                break;
            if (write(pipefd[1], "\n", 1) == -1)
                break;
        }
        close(pipefd[1]);
    }

    if (cmd->background)
    {
        return 0;
    }

    // wait for the child process to finish
    if (waitpid(pid, &status, 0) == -1)
    {
        print_sys_error("waitpid");
        return errno;
    }

    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    return 1;
}

/**
 * @brief Execute two commands connected by a pipe
 *
 * @param left The left command
 * @param right The right command
 * @return (int) The exit status of the right command
 */
static int execute_pipe(command_t *left, command_t *right)
{
    int pipefd[2];
    pid_t pid1, pid2;
    int status1 = 0, status2 = 0;

    if (pipe(pipefd) == -1)
    {
        print_sys_error("pipe");
        return errno;
    }

    // first processus: left |
    pid1 = fork();
    if (pid1 == -1)
    {
        print_sys_error("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return errno;
    }

    if (pid1 == 0)
    {
        // redirection of the input if specified for the left command: cat < in.txt |
        if (left->input_file)
        {
            int fd_in = open(left->input_file, O_RDONLY);
            if (fd_in == -1)
            {
                print_sys_error("open input");
                exit(EXIT_FAILURE);
            }
            if (dup2(fd_in, STDIN_FILENO) == -1)
            {
                print_sys_error("dup2 input");
                close(fd_in);
                exit(EXIT_FAILURE);
            }
            close(fd_in);
        }

        // output goes to the pipe
        if (dup2(pipefd[1], STDOUT_FILENO) == -1)
        {
            print_sys_error("dup2 pipe left");
            exit(EXIT_FAILURE);
        }

        close(pipefd[0]);
        close(pipefd[1]);

        execvp(left->argv[0], left->argv);
        print_sys_error("execvp left");
        exit(EXIT_FAILURE);
    }

    // second processus: ... | right
    pid2 = fork();
    if (pid2 == -1)
    {
        print_sys_error("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return errno;
    }

    if (pid2 == 0)
    {
        // input comes from the pipe
        if (dup2(pipefd[0], STDIN_FILENO) == -1)
        {
            print_sys_error("dup2 pipe right");
            exit(EXIT_FAILURE);
        }

        close(pipefd[0]);
        close(pipefd[1]);

        // redirection of the output if specified for the right command: ... | grep > out.txt
        if (right->output_file)
        {
            int flags = O_WRONLY | O_CREAT |
                        (right->append_output ? O_APPEND : O_TRUNC);
            int fd_out = open(right->output_file, flags, 0644);
            if (fd_out == -1)
            {
                print_sys_error("open output");
                exit(EXIT_FAILURE);
            }
            if (dup2(fd_out, STDOUT_FILENO) == -1)
            {
                print_sys_error("dup2 output");
                close(fd_out);
                exit(EXIT_FAILURE);
            }
            close(fd_out);
        }

        execvp(right->argv[0], right->argv);
        print_sys_error("execvp right");
        exit(EXIT_FAILURE);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);

    if (WIFEXITED(status2))
        return WEXITSTATUS(status2);
    return 1;
}

int execute_command(sequence_t *seq)
{

    int last_status = 0;

    if (!seq || seq->count <= 0)
        return 0;

    // if single built-in command, execute directly
    if (seq->count == 1 &&
        seq->commands[0].argc > 0 &&
        is_builtin(&seq->commands[0]))
    {
        return execute_builtin(&seq->commands[0]);
    }

    for (int i = 0; i < seq->count; ++i)
    {

        if (i > 0)
        {
            logical_operator_t op_prev = seq->operators[i - 1];

            if (op_prev == LOGICAL_AND && last_status != 0)
                continue;
            if (op_prev == LOGICAL_OR && last_status == 0)
                continue;
        }

        logical_operator_t op = (i < seq->count - 1)
                                    ? seq->operators[i]
                                    : LOGICAL_NONE;

        if (op == LOGICAL_PIPE && i + 1 < seq->count)
        {
            last_status = execute_pipe(&seq->commands[i],
                                       &seq->commands[i + 1]);
            i++; // skip the next command as it's part of the pipe
        }
        else
        {
            last_status = execute_one_command(&seq->commands[i]);
        }
    }
    return last_status;
}
