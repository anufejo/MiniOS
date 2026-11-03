#include <stdio.h>
#include "shell.h"

int main(void)
{
    char input[128];
    char output[SHELL_OUTPUT_SIZE];

    printf("MiniOS Shell Test\n");
    printf("Type 'help' to see available commands.\n\n");

    while (1)
    {
        printf("MiniOS> ");

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        ShellResult result = shell_execute(
            input, output, sizeof(output)
        );

        if (result == SHELL_CLEAR)
        {
            printf("\033[2J\033[H");
            continue;
        }

        if (output[0] != '\0')
            printf("%s\n", output);
    }

    return 0;
}