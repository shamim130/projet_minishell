/**
 * @file typedef.h
 * @brief Définition des structures de données du shell
 * @author 
 * @date 2025
 */

#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>



#define MAX_LINE 1024
#define MAX_ARGS 64

typedef struct s_command {
    char *argv[MAX_ARGS];
    int   argc;
} command_t;

#endif /* TYPEDEF_H */