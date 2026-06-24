#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/modulemgr.h>
// #include <psp2/message_dialog.h>
#include <psp2/display.h>
#include <psp2/ime_dialog.h>
#include <psp2/types.h>
#include <stdlib.h>
#include <string.h>


#include "keyboard.h"
#include "ascii_to_usb_hid.h"
#include "uapi/hidkeyboard_uapi.h"

/* Inter-character delay in microseconds.
 * The kernel thread polls every 10ms and needs one tick for press
 * and one for release. 50ms gives comfortable margin. */
#define INTER_CHAR_DELAY_US  50000

int keyboard_init(void)
{
    hidkeyboard_user_stop();   /* ignore error; might not be running yet */
    return hidkeyboard_user_start();
}

int keyboard_shutdown(void)
{
    return hidkeyboard_user_stop();
}

int keyboard_send_char(unsigned short c)
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

int keyboard_send_string(const char* str)
{
    if (!str)
        return -1;

    while (*str) {
        int ret = keyboard_send_char((unsigned char)*str);
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

int keyboard_send_modifier_key(char modifier, char key)
{
    return HidKeyBoardSendModifierAndKey(modifier, key);
}

static int utf8_to_utf16_bmp(const char *src, uint16_t *out, int max_units)
{
    int count = 0;
    while (*src) {
        if (count >= max_units)
            return -1;
        uint8_t c = (uint8_t)*src++;
        if (c < 0x80) {
            out[count++] = c;
            continue;
        }
        if ((c & 0xE0) == 0xC0) {
            uint8_t c2 = (uint8_t)*src++;
            if ((c2 & 0xC0) != 0x80)
                return -1;
            out[count++] = (uint16_t)(((c & 0x1F) << 6) | (c2 & 0x3F));
            continue;
        }
        if ((c & 0xF0) == 0xE0) {
            uint8_t c2 = (uint8_t)*src++;
            uint8_t c3 = (uint8_t)*src++;
            if ((c2 & 0xC0) != 0x80 || (c3 & 0xC0) != 0x80)
                return -1;
            uint16_t cp = (uint16_t)(((c & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F));
            if (cp >= 0xD800 && cp <= 0xDFFF)
                return -1;
            out[count++] = cp;
            continue;
        }
        return -1;
    }
    return count;
}

static int utf16_to_utf8_bmp(const uint16_t *src, char *out, int max_bytes)
{
    int count = 0;   /* number of bytes written */

    while (*src != 0) {
        uint16_t cp = *src;

        /* Reject surrogate halves (not allowed in BMP-only text) */
        if (cp >= 0xD800 && cp <= 0xDFFF)
            return -1;

        /* Determine required number of UTF-8 bytes */
        int needed;
        if (cp < 0x80)
            needed = 1;
        else if (cp < 0x800)
            needed = 2;
        else
            needed = 3;   /* cp <= 0xFFFF, already excluded surrogates */

        /* Check for buffer overflow */
        if (count + needed > max_bytes)
            return -1;

        /* Encode codepoint */
        if (needed == 1) {
            out[count++] = (char)cp;
        } else if (needed == 2) {
            out[count++] = (char)(0xC0 | (cp >> 6));
            out[count++] = (char)(0x80 | (cp & 0x3F));
        } else { /* needed == 3 */
            out[count++] = (char)(0xE0 | (cp >> 12));
            out[count++] = (char)(0x80 | ((cp >> 6) & 0x3F));
            out[count++] = (char)(0x80 | (cp & 0x3F));
        }

        src++;
    }

    return count;
}


int keyboard_prompt(const char *title,
                             const char *initial,
                             char *out, unsigned int out_len,
                             int max_chars)
{
    if (!out || out_len < 2 || max_chars <= 0) return -1;
    if (max_chars > SCE_IME_DIALOG_MAX_TEXT_LENGTH)
        max_chars = SCE_IME_DIALOG_MAX_TEXT_LENGTH;
 
    out[0] = '\0';
 
    /* Title and initial text need to be UTF-16. Fixed buffers are fine —
     * the IME has its own limits on these. */
    SceWChar16 title_w[SCE_IME_DIALOG_MAX_TITLE_LENGTH + 1];
    SceWChar16 initial_w[SCE_IME_DIALOG_MAX_TEXT_LENGTH + 1];
    memset(title_w, 0, sizeof(title_w));
    memset(initial_w, 0, sizeof(initial_w));
 
    if (title)
        utf8_to_utf16_bmp(title, title_w, SCE_IME_DIALOG_MAX_TITLE_LENGTH + 1);
    if (initial)
        utf8_to_utf16_bmp(initial, initial_w, SCE_IME_DIALOG_MAX_TEXT_LENGTH + 1);
 
    /* Result buffer: max_chars + 1 for NUL */
    SceWChar16 *result_w = (SceWChar16 *)malloc((max_chars + 1) * sizeof(SceWChar16));
    if (!result_w) return -2;
    memset(result_w, 0, (max_chars + 1) * sizeof(SceWChar16));
 
    /* Common dialog must be configured before any common dialog can run. */
    SceCommonDialogConfigParam config;
    sceCommonDialogConfigParamInit(&config);
    sceCommonDialogSetConfigParam(&config);
 
    SceImeDialogParam param;
    sceImeDialogParamInit(&param);
    param.supportedLanguages   = 0;                              /* allow all */
    param.languagesForced      = SCE_TRUE;
    param.type                 = SCE_IME_TYPE_DEFAULT;
    param.option               = 0;
    param.textBoxMode          = SCE_IME_DIALOG_TEXTBOX_MODE_DEFAULT;
    param.title                = title_w;
    param.maxTextLength        = (SceUInt32)max_chars;
    param.initialText          = initial_w;
    param.inputTextBuffer      = result_w;
 
    if (sceImeDialogInit(&param) < 0) {
        free(result_w);
        return -3;
    }
 
    /* Drive the common dialog system until the IME closes.
     * We blit the currently-displayed framebuffer back so the OS overlay
     * (the keyboard UI itself) is composited on top of it. */
    for (;;) {
        int status = sceImeDialogGetStatus();
        if (status == SCE_COMMON_DIALOG_STATUS_FINISHED) break;
 
        SceDisplayFrameBuf fb;
        memset(&fb, 0, sizeof(fb));
        fb.size = sizeof(fb);
        sceDisplayGetFrameBuf(&fb, SCE_DISPLAY_SETBUF_NEXTFRAME);
 
        SceCommonDialogUpdateParam up;
        memset(&up, 0, sizeof(up));
        up.renderTarget.colorFormat     = SCE_GXM_COLOR_FORMAT_A8B8G8R8;
        up.renderTarget.surfaceType     = SCE_GXM_COLOR_SURFACE_LINEAR;
        up.renderTarget.width           = fb.width;
        up.renderTarget.height          = fb.height;
        up.renderTarget.strideInPixels  = fb.pitch;
        up.renderTarget.colorSurfaceData = fb.base;
        sceCommonDialogUpdate(&up);
 
        sceKernelDelayThread(16000); /* ~60 Hz */
    }
 
    SceImeDialogResult res;
    memset(&res, 0, sizeof(res));
    sceImeDialogGetResult(&res);
 
    int rc = -4;
    if (res.button == SCE_IME_DIALOG_BUTTON_ENTER) {
        utf16_to_utf8_bmp(result_w, out, out_len);
        rc = 0;
    } else if (res.button == SCE_IME_DIALOG_BUTTON_CLOSE ||
               res.button == SCE_IME_DIALOG_BUTTON_NONE) {
        out[0] = '\0';
        rc = 1;
    }
 
    sceImeDialogTerm();
    free(result_w);
    return rc;
}
