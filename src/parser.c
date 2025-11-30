#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "../include/typedef.h"
#include "../include/parser.h"

static int is_logical(const char *s)
{
    return (strcmp(s, "&&") == 0 || strcmp(s, "||") == 0);
}

int parse_command(char *input, sequence_t *seq)
{
    if (!input || !seq) {
        errno = EFAULT;
        return -1;
    }

    memset(seq, 0, sizeof(*seq));

    int   cmd_index = 0;
    int   arg_index = 0;
    char *saveptr   = NULL;
    char *token     = strtok_r(input, " \t", &saveptr);

    while (token && cmd_index < MAX_COMMANDS) {

        /* opérateurs logiques && || */
        if (is_logical(token)) {
            if (arg_index == 0) {
                fprintf(stderr, "Erreur: operateur sans commande\n");
                errno = EINVAL;
                return -1;
            }

            seq->commands[cmd_index].argc = arg_index;
            seq->commands[cmd_index].argv[arg_index] = NULL;

            if (strcmp(token, "&&") == 0)
                seq->operators[cmd_index] = LOGICAL_AND;
            else
                seq->operators[cmd_index] = LOGICAL_OR;

            cmd_index++;
            arg_index = 0;
        }
        /* pipe | */
        else if (strcmp(token, "|") == 0) {
            if (arg_index == 0) {
                fprintf(stderr, "Erreur: | sans commande\n");
                errno = EINVAL;
                return -1;
            }

            seq->commands[cmd_index].argc = arg_index;
            seq->commands[cmd_index].argv[arg_index] = NULL;
            seq->operators[cmd_index] = LOGICAL_PIPE;

            cmd_index++;
            arg_index = 0;
        }
        /* arrière-plan & */
        else if (strcmp(token, "&") == 0) {
            if (arg_index == 0) {
                fprintf(stderr, "Erreur: & sans commande\n");
                errno = EINVAL;
                return -1;
            }

            seq->commands[cmd_index].background = 1;
            seq->commands[cmd_index].argc = arg_index;
            seq->commands[cmd_index].argv[arg_index] = NULL;

            cmd_index++;
            arg_index = 0;
        }
        /* redirections >, >>, <, << */
        else if (strcmp(token, ">") == 0 || strcmp(token, ">>") == 0) {
            command_t *cmd = &seq->commands[cmd_index];

            cmd->append_output = (strcmp(token, ">>") == 0);

            token = strtok_r(NULL, " \t", &saveptr);
            if (!token) {
                fprintf(stderr, "Erreur: nom de fichier manquant apres >\n");
                errno = EINVAL;
                return -1;
            }
            cmd->output_file = token;
        }
        else if (strcmp(token, "<") == 0) {
            command_t *cmd = &seq->commands[cmd_index];

            token = strtok_r(NULL, " \t", &saveptr);
            if (!token) {
                fprintf(stderr, "Erreur: nom de fichier manquant apres <\n");
                errno = EINVAL;
                return -1;
            }
            cmd->input_file = token;
        }
        else if (strcmp(token, "<<") == 0) {
            command_t *cmd = &seq->commands[cmd_index];

            token = strtok_r(NULL, " \t", &saveptr);
            if (!token) {
                fprintf(stderr, "Erreur: delimiteur manquant apres <<\n");
                errno = EINVAL;
                return -1;
            }
            cmd->heredoc_delim = token;
        }
        /* argument normal */
        else {
            if (arg_index >= MAX_ARGS - 1) {
                fprintf(stderr, "Erreur: trop d'arguments\n");
                errno = E2BIG;
                return -1;
            }
            seq->commands[cmd_index].argv[arg_index++] = token;
        }

        token = strtok_r(NULL, " \t", &saveptr);
    }

    /* opérateur final sans commande derrière: echo test &&, echo test ||, ls |  */
    if (arg_index == 0 && cmd_index > 0) {
        logical_operator_t last = seq->operators[cmd_index - 1];
        if (last == LOGICAL_AND || last == LOGICAL_OR || last == LOGICAL_PIPE) {
            fprintf(stderr, "Erreur: operateur final sans commande\n");
            errno = EINVAL;
            return -1;
        }
    }

    if (arg_index > 0) {
        seq->commands[cmd_index].argc = arg_index;
        seq->commands[cmd_index].argv[arg_index] = NULL;
        seq->count = cmd_index + 1;
    } else {
        seq->count = cmd_index;
    }

    if (seq->count == 0) {
        fprintf(stderr, "Erreur: aucune commande\n");
        errno = EINVAL;
        return -1;
    }

    return 0;
}