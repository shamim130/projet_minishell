



#ifndef ERROR_H
#define ERROR_H

/**
 * @brief Affiche un message d'erreur système basé sur errno
 * @param ctx texte décrivant le contexte (ex: "fork", "execvp")
 */
void print_sys_error(const char *ctx);

#endif
