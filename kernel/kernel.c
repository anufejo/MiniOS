#include "include/kernel.h"
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define VGA_MEMORY ((uint16_t*)0xB8000)
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

void print(const char *str, int row)
{
    int col = 0;

    while (*str)
    {
        VGA_MEMORY[row * SCREEN_WIDTH + col] =
            (uint16_t)(0x0F00 | *str);

        str++;
        col++;
    }
}

void kernel_main(void)
{
    print("================================", 3);
    print("          WELCOME TO MiniOS      ", 4);
    print("================================", 5);

    print("Kernel initialized successfully!", 8);
    print("Architecture : x86 (32-bit)", 10);
    print("Bootloader   : GRUB", 11);
    print("Status       : Running", 12);

    while (1)
    {
        __asm__ volatile ("hlt");
    }
}
