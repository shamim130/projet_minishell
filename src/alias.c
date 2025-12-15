/**
 * @file alias.c
 * @author Shamim SEDGHI , Mathys
 * @brief implementation of alias handling functions
 * @date december 2025
 * 
 * @copyright Copyright (c) 2025
 * 
 */




#include <stdio.h>
#include <string.h>
#include "../include/alias.h"

alias_t aliases[MAX_ALIAS];
int alias_count = 0;

int alias_add(const char *name, const char *value)
{
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(aliases[i].name, name) == 0) {
            strncpy(aliases[i].value, value, sizeof(aliases[i].value) - 1);
            return 0;
        }
    }

    if (alias_count >= MAX_ALIAS)
        return -1;

    strncpy(aliases[alias_count].name, name, sizeof(aliases[alias_count].name) - 1);
    strncpy(aliases[alias_count].value, value, sizeof(aliases[alias_count].value) - 1);
    alias_count++;
    return 0;
}


int alias_expand(char *line, size_t size)
{
    char cmd[64];
    int i = 0;

    // extract the first word from the line
    while (line[i] && line[i] != ' ' && line[i] != '\t') {
        cmd[i] = line[i];
        i++;
    }
    cmd[i] = '\0';

    for (int a = 0; a < alias_count; a++) {
        if (strcmp(cmd, aliases[a].name) == 0) {

            char tmp[512];

            // reconstruction of the line with the alias value
            char *rest = line + i;
            while (*rest == ' ' || *rest == '\t')
                rest++;

            if (*rest)
                snprintf(tmp, sizeof(tmp), "%s %s", aliases[a].value, rest);
            else
                snprintf(tmp, sizeof(tmp), "%s", aliases[a].value);

            strncpy(line, tmp, size - 1);
            line[size - 1] = '\0';
            break;
        }
    }
    return 0;
}

