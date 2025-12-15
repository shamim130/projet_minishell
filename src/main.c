/**
 * @file main.c
 * @author Shamim SEDGHI , Mathys
 * @brief main function of the shell
 * @date december 2025
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "../include/shell.h"
#include "../include/typedef.h"
#include "../include/error.h"
#include "../include/executer.h"
#include "../include/parser.h"
#include "../include/history.h"
#include <string.h>

int main(int argc, char *argv[])
{
    char line[MAX_LINE];
    sequence_t seq;

    /* FM06 MODE BATCH */
    if (argc == 3 && strcmp(argv[1], "-c") == 0)
    {
        strncpy(line, argv[2], sizeof(line) - 1);
        line[sizeof(line) - 1] = '\0';

        if (parse_command(line, &seq) == 0)
            execute_command(&seq);

        return 0;
    }

    /* MODE INTERACTIF */
    while (1)
    {
        print_prompt();

        if (!read_line(line, sizeof(line)))
            break;

        if (line[0] == '\0')
            continue;

        history_append(line);

        if (parse_command(line, &seq) < 0)
            continue;

        execute_command(&seq);
    }

    return 0;
}
