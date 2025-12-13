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

int main(void)
{
    char line[MAX_LINE];
    sequence_t seq;
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
