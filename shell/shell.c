#include "shell.h"
/* Calculate the length of a string. */
static unsigned int string_length(const char *str)
{
    unsigned int length = 0;

    while (str[length] != '\0')
    {
        length++;
    }

    return length;
}

/* Compare two strings. */
static int string_compare(const char *a, const char *b)
{
    unsigned int i = 0;

    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] != b[i])
            return 0;

        i++;
    }

    return a[i] == '\0' && b[i] == '\0';
}

/* Copy text into an output buffer safely. */
static void copy_text(char *output,
                      unsigned int output_size,
                      const char *text)
{
    unsigned int i = 0;

    if (output_size == 0)
    {
        return;
    }

    while (text[i] != '\0' && i < output_size - 1)
    {
        output[i] = text[i];
        i++;
    }

    output[i] = '\0';
}

/* Process a command entered by the user. */
ShellResult shell_execute(const char *input,
                          char *output,
                          unsigned int output_size)
{
    unsigned int start = 0;
    unsigned int end = 0;
    unsigned int command_length = 0;
    char command[32];
    unsigned int i;

    if (output_size == 0)
    {
        return SHELL_OK;
    }

    output[0] = '\0';

    if (input == 0)
    {
        copy_text(output, output_size, "Invalid input.");
        return SHELL_OK;
    }

    /* Ignore spaces at the beginning. */
    while (input[start] == ' ' || input[start] == '\t')
    {
        start++;
    }

    /* Ignore spaces at the end. */
    end = string_length(input);
    while (end > 0 && (input[end - 1] == '\n' || input[end - 1] == '\r'))
    end--;
    while (end > start &&
           (input[end - 1] == ' ' || input[end - 1] == '\t'))
    {
        end--;
    }

    if (start == end)
    {
        return SHELL_OK;
    }

    /* Extract the command name. */
    while (start + command_length < end &&
           input[start + command_length] != ' ' &&
           input[start + command_length] != '\t')
    {
        command_length++;
    }

    if (command_length >= sizeof(command))
    {
        copy_text(output, output_size, "Command name too long.");
        return SHELL_OK;
    }

    for (i = 0; i < command_length; i++)
    {
        command[i] = input[start + i];
    }

    command[command_length] = '\0';
    /* Find the beginning of the command argument. */
    start += command_length;

    while (start < end &&
           (input[start] == ' ' || input[start] == '\t'))
    {
        start++;
    }

    /* HELP command */
    if (string_compare(command, "help"))
    {
        copy_text(output, output_size,
            "Available commands:\n"
            "help    - Show available commands\n"
            "about   - Information about MiniOS\n"
            "version - Show OS version\n"
            "echo    - Display a message\n"
            "clear   - Clear the terminal");
    }

    /* ABOUT command */
    else if (string_compare(command, "about"))
    {
        copy_text(output, output_size,
            "MiniOS - Educational Operating System\n"
            "Developed by a team of 3 students.\n"
            "Built using C and Assembly.");
    }

    /* VERSION command */
    else if (string_compare(command, "version"))
    {
        copy_text(output, output_size,
            "MiniOS version 1.0");
    }

    /* ECHO command */
    else if (string_compare(command, "echo"))
    {
        if (start == end)
        {
            copy_text(output, output_size, "Usage: echo <message>");
        }
        else
        {
            unsigned int length = end - start;
            unsigned int copy_length = length;

            if (copy_length >= output_size)
            {
                copy_length = output_size - 1;
            }

            for (i = 0; i < copy_length; i++)
            {
                output[i] = input[start + i];
            }

            output[copy_length] = '\0';
        }
    }

    /* CLEAR command */
    else if (string_compare(command, "clear"))
    {
        return SHELL_CLEAR;
    }

    /* Unknown command */
    else
    {
        copy_text(output, output_size,
            "Unknown command. Type 'help' for assistance.");
    }

    return SHELL_OK;
}