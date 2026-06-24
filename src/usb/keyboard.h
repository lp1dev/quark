#ifndef KEYBOARD_H_
#define KEYBOARD_H_

/**
 * @brief Initializes the keyboard plugin and loads it into kernel memory.
 * 
 * @return 0 on success, < 0 on error.
 */
int keyboard_init(void);

/**
 * @brief Shuts down the keyboard plugin (unloads it from kernel).
 */
int keyboard_shutdown(void);

/**
 * @brief Sends a single character via USB HID.
 *
 * Handles regular characters, backspace, newline, tab, etc.
 *
 * @param character The character to send (e.g., 'A', '\n', '\b').
 * @return 0 on success, < 0 on error.
 */
int keyboard_send_char(unsigned short int character);

/**
 * @brief Sends a null-terminated string via USB HID.
 *
 * @param str The string to send.
 * @return 0 on success, < 0 on error.
 */
int keyboard_send_string(const char *str);

/**
 * @brief Sends a key with an optional modifier (low-level USB HID).
 *
 * @param modifier Modifier mask (e.g., KEY_MOD_LSHIFT).
 * @param key       USB HID key code (e.g., KEY_A).
 * @return 0 on success, < 0 on error.
 */
int keyboard_send_modifier_key(char modifier, char key);

/*


*/
int keyboard_prompt(const char *title,
                             const char *initial,
                             char *out, unsigned int out_len,
                             int max_chars);

#endif