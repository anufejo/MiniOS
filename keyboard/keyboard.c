#include "keyboard.h"

#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64

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

static const char normal_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', '\n', 0, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' '
};

static const char shifted_map[128] = {
    0,  27, '!', '@', '#', '$', '%', '^',
    '&', '*', '(', ')', '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    'O', 'P', '{', '}', '\n', 0, 'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
    '"', '~', 0, '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', 0, '*',
    0, ' '
};

static int shift_pressed = 0;

void keyboard_init(void)
{
    shift_pressed = 0;
}

char keyboard_getchar(void)
{
    unsigned char scan_code;

    while (1)
    {
        if (inb(KEYBOARD_STATUS_PORT) & 1)
        {
            scan_code = inb(KEYBOARD_DATA_PORT);

            /* Left or right Shift pressed. */
            if (scan_code == 0x2A || scan_code == 0x36)
            {
                shift_pressed = 1;
                continue;
            }

            /* Left or right Shift released. */
            if (scan_code == 0xAA || scan_code == 0xB6)
            {
                shift_pressed = 0;
                continue;
            }

            /* Ignore all other key-release codes. */
            if (scan_code & 0x80)
                continue;

            if (scan_code >= 128)
                continue;

            if (shift_pressed)
            {
                if (shifted_map[scan_code] != 0)
                    return shifted_map[scan_code];
            }
            else
            {
                if (normal_map[scan_code] != 0)
                    return normal_map[scan_code];
            }
        }
    }
}
