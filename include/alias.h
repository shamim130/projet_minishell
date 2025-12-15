/**
 * @file alias.h
 * @author Shamim SEDGHI , Mathys
 * @brief function prototypes for alias handling
 * @date december 2025
 */

#include <string.h>
#include "../include/typedef.h"

#ifndef ALIAS_H
#define ALIAS_H


#define MAX_ALIAS 64


/**
 * @brief Structure representing a command alias
 *
 */
typedef struct {
    char name[64];
    char value[256];
} alias_t;

extern alias_t aliases[MAX_ALIAS]; // array of aliases
extern int alias_count; // current number of aliases


/**
 * @brief Add a new alias
 *
 * @param name The name of the alias
 * @param value The value of the alias
 * @return (int) 0 on success, -1 on failure
 */
int alias_add(const char *name, const char *value);



/**
 * @brief Expand aliases in a command line
 *
 * @param line The command line to expand
 * @param size The size of the line buffer
 * @return (int) 0 on success, -1 on failure
 */
int alias_expand(char *line, size_t size);



#endif
