/**
 * @file history.h
 * @author Shamim SEDGHI , Mathys
 * @brief  function prototypes for command history handling
 * @date december 2025
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef HISTORY_H
#define HISTORY_H

/**
 * @brief Append a line to the history file
 *
 * @param line The line to append
 * @return (int) 0 on success, error code on failure
 */
int history_append(const char *line);

#endif
