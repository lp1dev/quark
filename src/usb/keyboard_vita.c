#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/modulemgr.h>
#include "keyboard_vita.h"
#include "uapi/hidkeyboard_uapi.h"
#include "ascii_to_usb_hid.h"
#include <psp2/types.h>

/* Inter-character delay in microseconds.
 * The kernel thread polls every 10ms and needs one tick for press
 * and one for release. 50ms gives comfortable margin. */
#define INTER_CHAR_DELAY_US  50000

int vita_keyboard_init(void)
{
    hidkeyboard_user_stop();   /* ignore error; might not be running yet */
    return hidkeyboard_user_start();
}

int vita_keyboard_shutdown(void)
{
    return hidkeyboard_user_stop();
}

int vita_keyboard_send_char(unsigned short c)
{
    /* Handle control characters with hardcoded HID codes */
    switch (c) {
        case '\b': return HidKeyBoardSendModifierAndKey(0x00, 0x2A); /* Backspace */
        case '\n': return HidKeyBoardSendModifierAndKey(0x00, 0x28); /* Enter */
        case '\r': return HidKeyBoardSendModifierAndKey(0x00, 0x28); /* CR -> Enter */
        case '\t': return HidKeyBoardSendModifierAndKey(0x00, 0x2B); /* Tab */
        case 0x1B: return HidKeyBoardSendModifierAndKey(0x00, 0x29); /* Esc */
        case 0x7F: return HidKeyBoardSendModifierAndKey(0x00, 0x4C); /* Del */
        default: break;
    }

    /* Fast path: printable ASCII via direct lookup table.
     * Table is indexed from 0x20 (space). [0]=modifier, [1]=keycode. */
    if (c >= 0x20 && c <= 0x7E) {
        unsigned char mod = ascii_to_hid_key_map[c - 0x20][0];
        unsigned char key = ascii_to_hid_key_map[c - 0x20][1];
        if (key != 0)
            return HidKeyBoardSendModifierAndKey(mod, key);
    }

    /* Fallback: layout-aware path for non-ASCII / accented chars */
    return HidKeyboardSendChar(c);
}

int vita_keyboard_send_string(const char* str)
{
    if (!str)
        return -1;

    while (*str) {
        int ret = vita_keyboard_send_char((unsigned char)*str);
        if (ret < 0)
            return ret;

        /* Wait for the kernel thread to fully complete press+release
         * before queuing the next character. Without this, the next
         * char overwrites the current one mid-cycle and gets lost. */
        sceKernelDelayThread(INTER_CHAR_DELAY_US);

        str++;
    }
    return 0;
}

int vita_keyboard_send_modifier_key(char modifier, char key)
{
    return HidKeyBoardSendModifierAndKey(modifier, key);
}