#ifndef TYPEDEF_H
#define TYPEDEF_H

#define MAX_LINE     1024
#define MAX_ARGS     64
#define MAX_COMMANDS 3
#define HISTORY_FILE ".my_shell_history"

typedef struct s_command {
    char *argv[MAX_ARGS];
    int   argc;

    char *input_file;      /* < */
    char *output_file;     /* > ou >> */
    int   append_output;  /* 1 si >>, 0 si > */
    int background;     /* 1 si exécution en arrière-plan (&) */
    char *heredoc_delim;   /* pour << */
} command_t;

typedef enum e_logical_operator {
    LOGICAL_NONE,
    LOGICAL_AND,
    LOGICAL_OR,
    LOGICAL_PIPE
} logical_operator_t;

typedef struct s_sequence {
    command_t          commands[MAX_COMMANDS];
    logical_operator_t operators[MAX_COMMANDS - 1];
    int                count;
} sequence_t;

#endif
