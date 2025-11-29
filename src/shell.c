/**
 * @file shell.c
 * @brief Implémentation des fonctions principales du shell
 * @author 
 * @date 2025
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "../include/shell.h"
#include "../include/error.h"


void print_prompt(void) {
    printf("Prompt> ");
    fflush(stdout);
}

int read_line(char *buffer, int size) {
    if (!fgets(buffer, size, stdin)) {
        if (ferror(stdin)) {
            print_sys_error("fgets");
        }
        return 0;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    return 1;
}

int parse_line(char *line, command_t *cmd) {
    int argc = 0;
    char *token = strtok(line, " ");

    while (token && argc < MAX_ARGS - 1) {
        cmd->argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    cmd->argv[argc] = NULL;
    cmd->argc = argc;
    return argc;
}

int execute_simple(command_t *cmd) {
    pid_t pid = fork();

    if (pid == -1) {
        print_sys_error("fork");
        return errno;
    }
    if (pid == 0) {
        execvp(cmd->argv[0], cmd->argv);
        print_sys_error("execvp");
        exit(EXIT_FAILURE);
    }
    int status;
    if (waitpid(pid, &status, 0) == -1) {
        print_sys_error("waitpid");
        return errno;
    }
    return status;
}