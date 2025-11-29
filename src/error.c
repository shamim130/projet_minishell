/**
 * @file error.c
 * @brief Gestion des erreurs via errno
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "error.h"

void print_sys_error(const char *ctx) {
    int err = errno;  /* sauvegarde au cas où */
    fprintf(stderr, "%s: %s\n", ctx, strerror(err));
}
