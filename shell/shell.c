#include "shell.h"
#include <limits.h>

/* Calculate the length of a string. */
static unsigned int string_length(const char *str)
{
    unsigned int length = 0;

    while (str[length] != '\0')
        length++;

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

/* Safely copy text into an output buffer. */
static void copy_text(char *output,
                      unsigned int output_size,
                      const char *text)
{
    unsigned int i = 0;

    if (output == 0 || output_size == 0)
        return;

    while (text[i] != '\0' && i < output_size - 1)
    {
        output[i] = text[i];
        i++;
    }

    output[i] = '\0';
}

/* Read a signed integer from the command. */
static int parse_integer(const char *input,
                         unsigned int *position,
                         unsigned int end,
                         int *value)
{
    unsigned long long number = 0;
    unsigned long long limit;
    unsigned int digits = 0;
    int sign = 1;

    while (*position < end &&
           (input[*position] == ' ' || input[*position] == '\t'))
        (*position)++;

    if (*position < end && input[*position] == '-')
    {
        sign = -1;
        (*position)++;
    }
    else if (*position < end && input[*position] == '+')
    {
        (*position)++;
    }

    limit = (sign == 1)
        ? (unsigned long long)INT_MAX
        : (unsigned long long)(-(long long)INT_MIN);

    while (*position < end &&
           input[*position] >= '0' &&
           input[*position] <= '9')
    {
        unsigned int digit = input[*position] - '0';

        if (number > (limit - digit) / 10)
            return 0;

        number = number * 10 + digit;
        digits++;
        (*position)++;
    }

    if (digits == 0)
        return 0;

    if (sign == -1)
        *value = (int)(-(long long)number);
    else
        *value = (int)number;

    return 1;
}

/* Convert an integer into text. */
static void integer_to_text(int number, char *text)
{
    char temp[32];
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned long long value;

    if (number < 0)
        value = (unsigned long long)(-(long long)number);
    else
        value = (unsigned long long)number;

    if (number < 0)
        text[j++] = '-';

    do
    {
        temp[i++] = (char)('0' + value % 10);
        value /= 10;
    }
    while (value > 0);

    while (i > 0)
        text[j++] = temp[--i];

    text[j] = '\0';
}

/* Display a message followed by an integer. */
static void number_message(char *output,
                           unsigned int output_size,
                           const char *message,
                           int number)
{
    char number_text[32];
    unsigned int i;
    unsigned int j = 0;

    integer_to_text(number, number_text);
    copy_text(output, output_size, message);

    i = string_length(output);

    while (number_text[j] != '\0' && i < output_size - 1)
        output[i++] = number_text[j++];

    output[i] = '\0';
}

/* Calculator command: calc 5 + 3 */
static void calculate(const char *input,
                      unsigned int start,
                      unsigned int end,
                      char *output,
                      unsigned int output_size)
{
    unsigned int position = start;
    int first;
    int second;
    int result;
    char operation;
    char number_text[32];
    long long answer;

    if (!parse_integer(input, &position, end, &first))
    {
        copy_text(output, output_size,
                  "Usage: calc <number> <operator> <number>");
        return;
    }

    while (position < end &&
           (input[position] == ' ' || input[position] == '\t'))
        position++;

    if (position >= end)
    {
        copy_text(output, output_size, "Missing operator.");
        return;
    }

    operation = input[position++];

    if (!parse_integer(input, &position, end, &second))
    {
        copy_text(output, output_size,
                  "Usage: calc <number> <operator> <number>");
        return;
    }

    while (position < end &&
           (input[position] == ' ' || input[position] == '\t'))
        position++;

    if (position != end)
    {
        copy_text(output, output_size, "Invalid calculator input.");
        return;
    }

    switch (operation)
    {
        case '+':
            answer = (long long)first + second;
            break;

        case '-':
            answer = (long long)first - second;
            break;

        case '*':
            answer = (long long)first * second;
            break;

        case '/':
            if (second == 0)
            {
                copy_text(output, output_size,
                          "Error: Division by zero.");
                return;
            }

            if (first == INT_MIN && second == -1)
            {
                copy_text(output, output_size,
                          "Error: Result is too large.");
                return;
            }

            answer = first / second;
            break;

        default:
            copy_text(output, output_size,
                      "Invalid operator. Use +, -, * or /.");
            return;
    }

    if (answer > INT_MAX || answer < INT_MIN)
    {
        copy_text(output, output_size,
                  "Error: Result is too large.");
        return;
    }

    result = (int)answer;
    integer_to_text(result, number_text);

    copy_text(output, output_size, "Result: ");

    {
        unsigned int i = string_length(output);
        unsigned int j = 0;

        while (number_text[j] != '\0' && i < output_size - 1)
            output[i++] = number_text[j++];

        output[i] = '\0';
    }
}

/* Process a command entered by the user. */
ShellResult shell_execute(const char *input,
                          char *output,
                          unsigned int output_size)
{
    unsigned int start = 0;
    unsigned int end;
    unsigned int command_length = 0;
    unsigned int i;
    char command[32];

    if (output == 0 || output_size == 0)
        return SHELL_OK;

    output[0] = '\0';

    if (input == 0)
    {
        copy_text(output, output_size, "Invalid input.");
        return SHELL_OK;
    }

    /* Ignore leading spaces and tabs. */
    while (input[start] == ' ' || input[start] == '\t')
        start++;

    end = string_length(input);

    /* Remove trailing newline characters. */
    while (end > start &&
           (input[end - 1] == '\n' || input[end - 1] == '\r'))
        end--;

    /* Ignore trailing spaces and tabs. */
    while (end > start &&
           (input[end - 1] == ' ' || input[end - 1] == '\t'))
        end--;

    if (start == end)
        return SHELL_OK;

    /* Extract command name. */
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
        command[i] = input[start + i];

    command[command_length] = '\0';

    /* Find command argument. */
    start += command_length;

    while (start < end &&
           (input[start] == ' ' || input[start] == '\t'))
        start++;

    /* HELP command. */
    if (string_compare(command, "help"))
    {
        copy_text(output, output_size,
            "Available commands:\n"
            "help    - Show available commands\n"
            "about   - Information about MiniOS\n"
            "version - Show OS version\n"
            "echo    - Display a message\n"
            "calc    - Perform basic calculations\n"
            "whoami  - Show current OS user\n"
            "clear   - Clear the terminal"
            "\nrepeat  - Repeat a message"
		"\ncount   - Count message characters"
		"\nreverse - Reverse a message"
		"\nupper   - Convert text to uppercase"
		"\nlower   - Convert text to lowercase"
		"\nsquare  - Calculate the square of a number"
		"\neven    - Check whether a number is even"
		"\nodd     - Check whether a number is odd");
    }

    /* ABOUT command. */
    else if (string_compare(command, "about"))
    {
        copy_text(output, output_size,
            "MiniOS - Educational Operating System\n"
            "Developed by a team of 3 students.\n"
            "Built using C and Assembly.");
    }

    /* VERSION command. */
    else if (string_compare(command, "version"))
    {
        copy_text(output, output_size, "MiniOS version 1.0");
    }

    /* ECHO command. */
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
                copy_length = output_size - 1;

            for (i = 0; i < copy_length; i++)
                output[i] = input[start + i];

            output[copy_length] = '\0';
        }
    }

    /* CALCULATOR command. */
    else if (string_compare(command, "calc"))
    {
        calculate(input, start, end, output, output_size);
    }

    /* WHOAMI command. */
    else if (string_compare(command, "whoami"))
    {
        copy_text(output, output_size, "MiniOS user");
    }
    
        /* REPEAT command. */
    else if (string_compare(command, "repeat"))
    {
        if (start == end)
            copy_text(output, output_size, "Usage: repeat <message>");
        else
        {
            for (i = 0; i < end - start && i < output_size - 1; i++)
                output[i] = input[start + i];

            output[i] = '\0';
        }
    }

    /* COUNT command. */
    else if (string_compare(command, "count"))
    {
        if (start == end)
        {
            copy_text(output, output_size, "Usage: count <message>");
        }
        else
        {
            number_message(output, output_size,
                           "Characters: ", (int)(end - start));
        }
    }

    /* REVERSE command. */
    else if (string_compare(command, "reverse"))
    {
        if (start == end)
        {
            copy_text(output, output_size, "Usage: reverse <message>");
        }
        else
        {
            unsigned int length = end - start;

            if (length >= output_size)
                length = output_size - 1;

            for (i = 0; i < length; i++)
                output[i] = input[end - 1 - i];

            output[length] = '\0';
        }
    }

    /* UPPER command. */
    else if (string_compare(command, "upper"))
    {
        if (start == end)
        {
            copy_text(output, output_size, "Usage: upper <message>");
        }
        else
        {
            unsigned int length = end - start;

            if (length >= output_size)
                length = output_size - 1;

            for (i = 0; i < length; i++)
            {
                char ch = input[start + i];

                if (ch >= 'a' && ch <= 'z')
                    ch = ch - 'a' + 'A';

                output[i] = ch;
            }

            output[length] = '\0';
        }
    }

    /* LOWER command. */
    else if (string_compare(command, "lower"))
    {
        if (start == end)
        {
            copy_text(output, output_size, "Usage: lower <message>");
        }
        else
        {
            unsigned int length = end - start;

            if (length >= output_size)
                length = output_size - 1;

            for (i = 0; i < length; i++)
            {
                char ch = input[start + i];

                if (ch >= 'A' && ch <= 'Z')
                    ch = ch - 'A' + 'a';

                output[i] = ch;
            }

            output[length] = '\0';
        }
    }

    /* SQUARE, EVEN AND ODD commands. */
    else if (string_compare(command, "square") ||
             string_compare(command, "even") ||
             string_compare(command, "odd"))
    {
        unsigned int position = start;
        int number;

        if (!parse_integer(input, &position, end, &number))
        {
            copy_text(output, output_size,
                      "Usage: enter a valid integer.");
        }
        else
        {
            while (position < end &&
                   (input[position] == ' ' || input[position] == '\t'))
                position++;

            if (position != end)
            {
                copy_text(output, output_size, "Invalid number.");
            }
            else if (string_compare(command, "square"))
            {
                long long result = (long long)number * number;

                if (result > INT_MAX)
                    copy_text(output, output_size, "Result is too large.");
                else
                    number_message(output, output_size,
                                   "Square: ", (int)result);
            }
            else if (string_compare(command, "even"))
            {
                copy_text(output, output_size,
                    number % 2 == 0 ? "The number is even."
                                    : "The number is not even.");
            }
            else
            {
                copy_text(output, output_size,
                    number % 2 != 0 ? "The number is odd."
                                    : "The number is not odd.");
            }
        }
    }

    /* CLEAR command. */
    else if (string_compare(command, "clear"))
    {
        return SHELL_CLEAR;
    }

    /* Unknown command. */
    else
    {
        copy_text(output, output_size,
            "Unknown command. Type 'help' for assistance.");
    }

    return SHELL_OK;
}
