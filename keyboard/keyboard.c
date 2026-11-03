
#include "keyboard.h"

/* PS/2 keyboard controller ports */
#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64

/* Read a byte from an x86 hardware port */
static inline unsigned char inb(unsigned short port)
{
    unsigned char result;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(result)
        : "Nd"(port)
    );

    return result;
}

/* Basic US keyboard scan-code map (Set 1) */
static const char keyboard_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', '\n', 0, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' '
};

void keyboard_init(void)
{
    /* Polling driver: no interrupt setup needed yet. */
}

char keyboard_getchar(void)
{
    unsigned char scan_code;

    while (1)
    {
        /* Wait until the controller has data */
        if (inb(KEYBOARD_STATUS_PORT) & 1)
        {
            scan_code = inb(KEYBOARD_DATA_PORT);

            /* Ignore key-release scan codes */
            

            /* Ignore unmapped scan codes */
            if (scan_code >= 128)
                continue;

            if (keyboard_map[scan_code] != 0)
                return keyboard_map[scan_code];
        }
    }
}
