/**
 * @file history.c
 * @author Shamim SEDGHI , Mathys
 * @brief  handling of command history
 * @date december 2025
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "../include/typedef.h"
#include "../include/error.h"
#include "../include/history.h"

int history_append(const char *line)
{
    int fd;
    ssize_t len;
    ssize_t written;

    if (!line || line[0] == '\0')
        return 0;

    // Open the history file in append mode (or create it if it doesn't exist)
    fd = open(HISTORY_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        print_sys_error("open history");
        return errno;
    }

    len = (ssize_t)strlen(line);
    written = write(fd, line, len);
    if (written == -1)
    {
        print_sys_error("write history");
        close(fd);
        return errno;
    }

    if (write(fd, "\n", 1) == -1)
    {
        print_sys_error("write history newline");
        close(fd);
        return errno;
    }

    close(fd);
    return 0;
}
