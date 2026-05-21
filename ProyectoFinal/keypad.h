/*
 * keypad.h
 *
 * Created: 20/05/2026 04:08:50 p. m.
 *  Author: astri
 */

#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdint.h>

void keypad_init(void);
char keypad_scan(void);
char keypad_get_pressed_key(void);

#endif /* KEYPAD_H */
