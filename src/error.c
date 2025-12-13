/**
 * @file error.c
 * @author Shamim SEDGHI , Mathys
 * @brief error handling functions
 * @date december 2025
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "error.h"

void print_sys_error(const char *ctx)
{
    int err = errno;
    fprintf(stderr, "%s: %s\n", ctx, strerror(err));
}
