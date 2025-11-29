/**
 * @file main.c
 * @brief Point d'entrée du shell simple
 * @author 
 * @date 2025
 */

#include <stdio.h>
#include "../include/shell.h"
#include "../include/typedef.h"

int main(void) {
    char line[MAX_LINE];
    command_t cmd;

    while (1) {
        print_prompt();
        if (!read_line(line, sizeof(line)))
            break;
        if (line[0] == '\0')
            continue;

        int argc = parse_line(line, &cmd);
        if (argc == 0)
            continue;

        execute_simple(&cmd);
    }
    return 0;
}