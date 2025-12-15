/**
 * @file built_in.c
 * @author Shamim SEDGHI , Mathys
 * @brief Commandes internes du shell (builtins)
 * @date december 2025
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include "../include/typedef.h"
#include "../include/error.h"
#include "../include/built_in.h"
#include "../include/alias.h"
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
static char previous_dir[PATH_MAX] = "";

int is_builtin(command_t *cmd)
{
    if (!cmd || cmd->argc == 0 || cmd->argv[0] == NULL)
        return 0;

    return (
        strcmp(cmd->argv[0], "cd") == 0 ||
        strcmp(cmd->argv[0], "pwd") == 0 ||
        strcmp(cmd->argv[0], "pwd_sys") == 0 ||
        strcmp(cmd->argv[0], "exit") == 0 ||
        strcmp(cmd->argv[0], "echo") == 0 ||
        strcmp(cmd->argv[0], "echo_sys") == 0 ||
        strcmp(cmd->argv[0], "ls_mon_shell") == 0 ||
        strcmp(cmd->argv[0], "alias") == 0  ||
        strcmp(cmd->argv[0], "unalias") == 0 );
}

/**
 * @brief implementaiton of the built-in command 'ls_mon_shell' for listing files of our version of main local directory
 *
 * @param cmd The command to execute
 * @return (int) The exit status of the command
 */
static int builtin_ls(command_t *cmd)
{
    const char *path = ".";
    DIR *dir;
    struct dirent *entry;

    if (cmd->argc >= 2)
        path = cmd->argv[1];

    dir = opendir(path);
    if (!dir)
    {
        print_sys_error("opendir");
        return errno;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        printf("%s  ", entry->d_name);
    }
    printf("\n");
    closedir(dir);
    return 0;
}

/**
 * @brief implementation of the built-in command 'cd' for changing the current working directory
 *
 * @param cmd The command to execute
 * @return (int) The exit status of the command
 */
static int builtin_cd(command_t *cmd)
{
    const char *path = NULL;
    char current_dir[PATH_MAX];


    // get the current directory
    if (!getcwd(current_dir, sizeof(current_dir)))
    {
        print_sys_error("getcwd");
        return errno;
    }

    // no argument : go to HOME directory
    if (cmd->argc < 2)
    {
        path = getenv("HOME");
        if (!path)
        {
            fprintf(stderr, "cd: HOME non definie\n");
            errno = ENOENT;
            return -1;
        }

        // argument is "-" : go to previous directory
    }
    else if (strcmp(cmd->argv[1], "-") == 0)
    {
        if (previous_dir[0] == '\0')
        {
            fprintf(stderr, "cd: OLDPWD non defini\n");
            errno = ENOENT;
            return -1;
        }
        path = previous_dir;
        printf("%s\n", path);
    }
    else
    {
        path = cmd->argv[1]; // argument is the target directory
    }

    // attempt to change directory
    if (chdir(path) == -1)
    {
        fprintf(stderr, "cd: %s: ", path);
        print_sys_error("chdir");
        return errno;
    }

    // update previous_dir
    strncpy(previous_dir, current_dir, sizeof(previous_dir) - 1);
    previous_dir[sizeof(previous_dir) - 1] = '\0';

    return 0;
}

/**
 * @brief implementation of the built-in command 'pwd' for printing the current working directory
 *
 * @param cmd The command to execute
 * @return (int) The exit status of the command
 */
static int builtin_pwd(command_t *cmd)
{
    char cwd[PATH_MAX];

    (void)cmd;

    if (!getcwd(cwd, sizeof(cwd)))
    {
        print_sys_error("getcwd");
        return errno;
    }

    printf("%s\n", cwd);
    return 0;
}

/**
 * @brief implementation of the built-in command 'echo' for printing arguments to standard output
 *
 * @param cmd The command to execute
 * @return (int) The exit status of the command
 */
static int builtin_echo(command_t *cmd)
{

    for (int i = 1; i < cmd->argc; ++i)
    {
        fputs(cmd->argv[i], stdout);
        if (i < cmd->argc - 1)
            fputc(' ', stdout);
    }
    fputc('\n', stdout);
    return 0;
}

/**
 * @brief implementation of the built-in command 'alias' for defining command aliases
 *
 * @param cmd The command to execute
 * @return (int) The exit status of the command
 */
static int builtin_alias(command_t *cmd)
{
    if (cmd->argc < 3)
    {
        fprintf(stderr, "usage: alias name command...\n");
        errno = EINVAL;
        return 1;
    }

    char value[256] = {0};

    // concatenate command arguments into a single string
    for (int i = 2; i < cmd->argc; i++)
    {
        strcat(value, cmd->argv[i]);
        if (i < cmd->argc - 1)
            strcat(value, " ");
    }

    alias_add(cmd->argv[1], value);
    return 0;
}

static int builtin_alias_list(void)
{
    for (int i = 0; i < alias_count; i++)
        printf("alias %s='%s'\n", aliases[i].name, aliases[i].value);
    return 0;
}

/**
 * @brief implementation of the built-in command 'unalias' for removing command aliases
 *
 * @param cmd The command to execute
 * @return (int) The exit status of the command
 */
static int builtin_unalias(command_t *cmd)
{
    if (cmd->argc != 2) {
        fprintf(stderr, "usage: unalias name\n");
        errno = EINVAL;
        return 1;
    }

    for (int i = 0; i < alias_count; i++) {
        if (strcmp(aliases[i].name, cmd->argv[1]) == 0) {
            for (int j = i; j < alias_count - 1; j++)
                aliases[j] = aliases[j + 1];
            alias_count--;
            return 0;
        }
    }

    fprintf(stderr, "unalias: %s: not found\n", cmd->argv[1]);
    errno = ENOENT;
    return 1;
}

int execute_builtin(command_t *cmd)
{
    if (strcmp(cmd->argv[0], "cd") == 0)
    {
        return builtin_cd(cmd);
    }
    if (strcmp(cmd->argv[0], "pwd") == 0)
    {
        return builtin_pwd(cmd);
    }

    if (strcmp(cmd->argv[0], "exit") == 0)
    {
        int status = 0;

        if (cmd->argc >= 2)
            status = atoi(cmd->argv[1]);
        exit(status);
    }
    if (strcmp(cmd->argv[0], "echo") == 0)
        return builtin_echo(cmd);
    if (strcmp(cmd->argv[0], "ls_mon_shell") == 0)

        return builtin_ls(cmd);
    if (strcmp(cmd->argv[0], "alias") == 0)
    {
        if (cmd->argc == 1)
            return builtin_alias_list();
        return builtin_alias(cmd);
    }

    if (strcmp(cmd->argv[0], "unalias") == 0)
    return builtin_unalias(cmd);


    return 0;
}
