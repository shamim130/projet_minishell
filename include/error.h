/**
 * @file error.h
 * @author Shamim SEDGHI , Mathys
 * @brief Function prototypes for error handling
 * @date december 2025
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef ERROR_H
#define ERROR_H

/**
 * @brief Print a system error message based on the current value of errno
 *
 * @param ctx The context message to display before the error description
 */
void print_sys_error(const char *ctx);

#endif
