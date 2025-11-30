/**
 * @file executer.c
 * @brief Implémentation des fonctions d'exécution des commandes
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

static int execute_one_command(command_t *cmd)
{
    pid_t pid;
    int   status;
    int   pipefd[2] = { -1, -1 };

    if (!cmd || cmd->argc == 0 || cmd->argv[0] == NULL)
        return 0;

    /* si heredoc, on prépare un pipe */
    if (cmd->heredoc_delim) {
        if (pipe(pipefd) == -1) {
            print_sys_error("pipe");
            return errno;
        }
    }

    pid = fork();
    if (pid == -1) {
        print_sys_error("fork");
        if (pipefd[0] != -1) {
            close(pipefd[0]);
            close(pipefd[1]);
        }
        return errno;
    }

    if (pid == 0) {
        /* fils */

        if (cmd->heredoc_delim) {
            /* lire depuis l'extrémité lecture du pipe */
            close(pipefd[1]);
            if (dup2(pipefd[0], STDIN_FILENO) == -1) {
                print_sys_error("dup2 heredoc");
                close(pipefd[0]);
                exit(EXIT_FAILURE);
            }
            close(pipefd[0]);
        } else if (cmd->input_file) {
            int fd_in = open(cmd->input_file, O_RDONLY);
            if (fd_in == -1) {
                print_sys_error("open input");
                exit(EXIT_FAILURE);
            }
            if (dup2(fd_in, STDIN_FILENO) == -1) {
                print_sys_error("dup2 input");
                close(fd_in);
                exit(EXIT_FAILURE);
            }
            close(fd_in);
        }

        if (cmd->output_file) {
            int flags = O_WRONLY | O_CREAT |
                        (cmd->append_output ? O_APPEND : O_TRUNC);
            int fd_out = open(cmd->output_file, flags, 0644);
            if (fd_out == -1) {
                print_sys_error("open output");
                exit(EXIT_FAILURE);
            }
            if (dup2(fd_out, STDOUT_FILENO) == -1) {
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

    /* parent */

    if (cmd->heredoc_delim) {
        char buffer[1024];

        close(pipefd[0]); /* on n’utilise que l’écriture ici */

        while (1) {
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

    if (cmd->background) {
        return 0;
    }

    if (waitpid(pid, &status, 0) == -1) {
        print_sys_error("waitpid");
        return errno;
    }

    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    return 1;
}

static int execute_pipe(command_t *left, command_t *right)
{
    int   pipefd[2];
    pid_t pid1, pid2;
    int   status1 = 0, status2 = 0;

    if (pipe(pipefd) == -1) {
        print_sys_error("pipe");
        return errno;
    }

    /* premier processus: left | */
    pid1 = fork();
    if (pid1 == -1) {
        print_sys_error("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return errno;
    }

    if (pid1 == 0) {
        /* redirection d'entrée éventuelle pour la commande de gauche: cat < in.txt | ... */
        if (left->input_file) {
            int fd_in = open(left->input_file, O_RDONLY);
            if (fd_in == -1) {
                print_sys_error("open input");
                exit(EXIT_FAILURE);
            }
            if (dup2(fd_in, STDIN_FILENO) == -1) {
                print_sys_error("dup2 input");
                close(fd_in);
                exit(EXIT_FAILURE);
            }
            close(fd_in);
        }

        /* la sortie va dans le pipe */
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            print_sys_error("dup2 pipe left");
            exit(EXIT_FAILURE);
        }

        close(pipefd[0]);
        close(pipefd[1]);

        execvp(left->argv[0], left->argv);
        print_sys_error("execvp left");
        exit(EXIT_FAILURE);
    }

    /* second processus: ... | right */
    pid2 = fork();
    if (pid2 == -1) {
        print_sys_error("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return errno;
    }

    if (pid2 == 0) {
        /* l'entrée vient du pipe */
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            print_sys_error("dup2 pipe right");
            exit(EXIT_FAILURE);
        }

        close(pipefd[0]);
        close(pipefd[1]);

        /* redirection de sortie éventuelle pour la commande de droite: ... | cat > out.txt */
        if (right->output_file) {
            int flags = O_WRONLY | O_CREAT |
                        (right->append_output ? O_APPEND : O_TRUNC);
            int fd_out = open(right->output_file, flags, 0644);
            if (fd_out == -1) {
                print_sys_error("open output");
                exit(EXIT_FAILURE);
            }
            if (dup2(fd_out, STDOUT_FILENO) == -1) {
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

    /* builtin simple: seulement cd tout seul, sans pipes ni && || */
    if (seq->count == 1 &&
        seq->commands[0].argc > 0 &&
        is_builtin(&seq->commands[0])) {
        return execute_builtin(&seq->commands[0]);
    }

    for (int i = 0; i < seq->count; ++i) {

        if (i > 0) {
            logical_operator_t op_prev = seq->operators[i - 1];

            if (op_prev == LOGICAL_AND && last_status != 0)
                continue;
            if (op_prev == LOGICAL_OR && last_status == 0)
                continue;
        }

        logical_operator_t op = (i < seq->count - 1)
                                ? seq->operators[i]
                                : LOGICAL_NONE;

        if (op == LOGICAL_PIPE && i + 1 < seq->count) {
            last_status = execute_pipe(&seq->commands[i],
                                       &seq->commands[i + 1]);
            i++; /* on saute la commande suivante, déjà exécutée avec le pipe */
        } else {
            last_status = execute_one_command(&seq->commands[i]);
        }
    }
    return last_status;
}
