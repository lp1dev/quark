#include "keyboard.h"

/*
This is not actually supported on Linux at the moment, 
these are just empty stubs.

*/
int keyboard_init(void)
{
    return 0;
}

int keyboard_shutdown(void)
{
    return 0;
}

int keyboard_send_char(unsigned short c)
{
    return 0;
}

int keyboard_send_string(const char* str)
{
    return 0;
}

int keyboard_send_modifier_key(char modifier, char key)
{
    return 0;
}

int keyboard_prompt(const char *title,
                             const char *initial,
                             char *out, unsigned int out_len,
                             int max_chars)
{
    return 0;
}