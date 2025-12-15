#include <string.h>
#include <stdio.h>
#include "../include/alias.h"

#define MAX_ALIAS 32
#define MAX_ALIAS_NAME 64
#define MAX_ALIAS_VALUE 256

typedef struct
{
    char name[MAX_ALIAS_NAME];
    char value[MAX_ALIAS_VALUE];
} alias_t;

static alias_t aliases[MAX_ALIAS];
static int alias_count = 0;

const char *alias_get(const char *name)
{
    for (int i = 0; i < alias_count; i++)
    {
        if (strcmp(aliases[i].name, name) == 0)
            return aliases[i].value;
    }
    return NULL;
}

int alias_set(const char *name, const char *value)
{
    for (int i = 0; i < alias_count; i++)
    {
        if (strcmp(aliases[i].name, name) == 0)
        {
            strncpy(aliases[i].value, value, MAX_ALIAS_VALUE);
            return 0;
        }
    }

    if (alias_count >= MAX_ALIAS)
        return -1;

    strncpy(aliases[alias_count].name, name, MAX_ALIAS_NAME);
    strncpy(aliases[alias_count].value, value, MAX_ALIAS_VALUE);
    alias_count++;
    return 0;
}

void alias_print_all(void)
{
    for (int i = 0; i < alias_count; i++)
        printf("alias %s=%s\n", aliases[i].name, aliases[i].value);
}
