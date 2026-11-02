#ifndef SHELL_H
#define SHELL_H

#define SHELL_OUTPUT_SIZE 512

typedef enum
{
    SHELL_OK,
    SHELL_CLEAR
} ShellResult;

/* Process a command entered by the user. */
ShellResult shell_execute(
    const char *input,
    char *output,
    unsigned int output_size
);

#endif