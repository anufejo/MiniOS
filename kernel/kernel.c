#include "include/kernel.h"
#include "shell.h"
#include "keyboard.h"

typedef unsigned short uint16_t;

#define VGA_MEMORY ((volatile uint16_t*)0xB8000)
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define INPUT_SIZE 128

static unsigned int cursor_row = 0;
static unsigned int cursor_col = 0;

/* Clear the VGA text screen. */
static void screen_clear(void)
{
    unsigned int i;

    for (i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
        VGA_MEMORY[i] = (uint16_t)(0x0F00 | ' ');

    cursor_row = 0;
    cursor_col = 0;
}

/* Scroll the screen when the bottom is reached. */
static void screen_scroll(void)
{
    unsigned int row;
    unsigned int col;

    for (row = 1; row < SCREEN_HEIGHT; row++)
    {
        for (col = 0; col < SCREEN_WIDTH; col++)
        {
            VGA_MEMORY[(row - 1) * SCREEN_WIDTH + col] =
                VGA_MEMORY[row * SCREEN_WIDTH + col];
        }
    }

    for (col = 0; col < SCREEN_WIDTH; col++)
        VGA_MEMORY[(SCREEN_HEIGHT - 1) * SCREEN_WIDTH + col] =
            (uint16_t)(0x0F00 | ' ');

    cursor_row = SCREEN_HEIGHT - 1;
}

/* Display one character. */
static void screen_put(char c)
{
    if (c == '\n')
    {
        cursor_col = 0;
        cursor_row++;
    }
    else if (c == '\r')
    {
        cursor_col = 0;
    }
    else if (c == '\b')
    {
        if (cursor_col > 0)
        {
            cursor_col--;
            VGA_MEMORY[cursor_row * SCREEN_WIDTH + cursor_col] =
                (uint16_t)(0x0F00 | ' ');
        }
    }
    else
    {
        VGA_MEMORY[cursor_row * SCREEN_WIDTH + cursor_col] =
            (uint16_t)(0x0F00 | (unsigned char)c);

        cursor_col++;

        if (cursor_col >= SCREEN_WIDTH)
        {
            cursor_col = 0;
            cursor_row++;
        }
    }

    if (cursor_row >= SCREEN_HEIGHT)
        screen_scroll();
}

/* Display a complete string. */
static void screen_write(const char *str)
{
    while (*str)
    {
        screen_put(*str);
        str++;
    }
}

/* Keep the existing kernel print interface. */
void print(const char *str, int row)
{
    int col = 0;

    while (*str && col < SCREEN_WIDTH && row >= 0 &&
           row < SCREEN_HEIGHT)
    {
        VGA_MEMORY[row * SCREEN_WIDTH + col] =
            (uint16_t)(0x0F00 | (unsigned char)*str);

        str++;
        col++;
    }
}

/* Display the command prompt. */
static void show_prompt(void)
{
    screen_write("MiniOS> ");
}

/* Main kernel function. */
void kernel_main(void)
{
    char input[INPUT_SIZE];
    char output[SHELL_OUTPUT_SIZE];
    unsigned int length;
    char key;
    ShellResult result;

    keyboard_init();
    screen_clear();

    screen_write("================================\n");
    screen_write("       WELCOME TO MiniOS\n");
    screen_write("================================\n");
    screen_write("MiniOS shell initialized!\n");
    screen_write("Type 'help' to see commands.\n\n");

    while (1)
    {
        show_prompt();
        length = 0;

        while (1)
        {
            key = keyboard_getchar();

            /* Enter key: execute the command. */
            if (key == '\n')
            {
                screen_put('\n');
                input[length] = '\0';
                break;
            }

            /* Backspace key. */
            if (key == '\b')
            {
                if (length > 0)
                {
                    length--;
                    screen_put('\b');
                }
                continue;
            }

            /* Add a character if there is buffer space. */
            if (length < INPUT_SIZE - 1)
            {
                input[length++] = key;
                screen_put(key);
            }
        }

        output[0] = '\0';

        result = shell_execute(
            input,
            output,
            sizeof(output)
        );

        if (result == SHELL_CLEAR)
        {
            screen_clear();
            continue;
        }

        if (output[0] != '\0')
        {
            screen_write(output);
            screen_put('\n');
        }
    }
}
