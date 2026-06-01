/*
 * keypad.c
 *
 * Created: 20/05/2026 04:10:12 p. m.
 *  Author: astri
 */

#include "keypad.h"
#include "config.h"
#include "timers.h"
#include <util/delay.h>

void keypad_init(void)
{
    // Todas las filas como salidas
    KBD_ROW_DDR |= (1 << KBD_ROW1) | (1 << KBD_ROW2) | (1 << KBD_ROW3) | (1 << KBD_ROW4);
    
    // Las ponemos en HIGH inicialmente
    KBD_ROW_PORT |= (1 << KBD_ROW1) | (1 << KBD_ROW2) | (1 << KBD_ROW3) | (1 << KBD_ROW4);

    // Columnas 1 y 2 (PORTD) como entradas
    KBD_COL_DDR &= ~((1 << KBD_COL1) | (1 << KBD_COL2));
    KBD_COL_PORT |= (1 << KBD_COL1) | (1 << KBD_COL2); // Pull-ups

    // Columnas 3 y 4 (PORTB) como entradas
    KBD_COL_B_DDR &= ~((1 << KBD_COL3) | (1 << KBD_COL4));
    KBD_COL_B_PORT |= (1 << KBD_COL3) | (1 << KBD_COL4); // Pull-ups
}

char keypad_scan(void)
{
    char keys[4][4] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };

    KBD_ROW_PORT |= (1 << KBD_ROW1) | (1 << KBD_ROW2) | (1 << KBD_ROW3) | (1 << KBD_ROW4);

    for (uint8_t r = 0; r < 4; r++) {
        // Ponemos la fila actual en LOW
        if (r == 0) KBD_ROW_PORT &= ~(1 << KBD_ROW1);
        else if (r == 1) KBD_ROW_PORT &= ~(1 << KBD_ROW2);
        else if (r == 2) KBD_ROW_PORT &= ~(1 << KBD_ROW3);
        else if (r == 3) KBD_ROW_PORT &= ~(1 << KBD_ROW4);

        _delay_us(10); // Un poco mas de tiempo de estabilizacion

        uint8_t col1 = !(KBD_COL_PIN & (1 << KBD_COL1));
        uint8_t col2 = !(KBD_COL_PIN & (1 << KBD_COL2));
        uint8_t col3 = !(KBD_COL_B_PIN & (1 << KBD_COL3));
        uint8_t col4 = !(KBD_COL_B_PIN & (1 << KBD_COL4));

        // Devolvemos la fila a HIGH
        if (r == 0) KBD_ROW_PORT |= (1 << KBD_ROW1);
        else if (r == 1) KBD_ROW_PORT |= (1 << KBD_ROW2);
        else if (r == 2) KBD_ROW_PORT |= (1 << KBD_ROW3);
        else if (r == 3) KBD_ROW_PORT |= (1 << KBD_ROW4);

        if (col1) return keys[r][0];
        if (col2) return keys[r][1];
        if (col3) return keys[r][2];
        if (col4) return keys[r][3];
    }

    return 0;
}

char keypad_get_pressed_key(void)
{
    static char last_key = 0;
    static uint32_t last_scan_ms = 0;
    
    uint32_t current_time = millis();
    if (current_time - last_scan_ms < 20) {
        return 0;
    }
    last_scan_ms = current_time;

    char current_key = keypad_scan();

    if (current_key != last_key) {
        last_key = current_key;
        if (current_key != 0) {
            return current_key;
        }
    }

    return 0;
}
