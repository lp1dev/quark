#include "keyboard_vita.h"

/*
This is not actually supported on Linux at the moment, 
these are just empty stubs.

*/
int vita_keyboard_init(void)
{
    return 0;
}

int vita_keyboard_shutdown(void)
{
    return 0;
}

int vita_keyboard_send_char(unsigned short c)
{
    return 0;
}

int vita_keyboard_send_string(const char* str)
{
    return 0;
}

int vita_keyboard_send_modifier_key(char modifier, char key)
{
    return 0;
}