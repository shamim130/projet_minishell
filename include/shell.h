
/**
 * @file shell.h
 * @brief Fonctions principales du shell
 * @author 
 * @date 2025
 */

#ifndef SHELL_H
#define SHELL_H
#include "typedef.h"

#define MAX_LINE 1024
#define MAX_ARGS 64


/**
 * @brief Affiche l'invite du shell
 * 
 */
void print_prompt(void);


/**
 * @brief Lit une ligne de l'entrée standard
 * 
 * @param buffer Le buffer où stocker la ligne lue
 * @param size La taille du buffer
 * @return int 1 si une ligne a été lue, 0 en cas de EOF
 */
int  read_line(char *buffer, int size);

/**
 * @brief Analyse une ligne en arguments
 * 
 * @param line La ligne à analyser
 * @param argv Le tableau où stocker les arguments
 * @param max_args Le nombre maximum d'arguments
 * @return int Le nombre d'arguments analysés
 */
int  parse_line(char *line, command_t *cmd);
/**
 * @brief Exécute une commande simple
 * 
 * @param argv Le tableau des arguments de la commande
 * @return int Le statut de sortie de la commande
 */
int  execute_simple(command_t *cmd);

#endif /* SHELL_H */