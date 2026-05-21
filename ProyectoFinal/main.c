/*
 * main.c
 *
 * Created: 20/05/2026 05:45:30 p. m.
 *  Author: astri
 */

#include "config.h"
#include "adc.h"
#include "timers.h"
#include "buzzer.h"
#include "mux.h"
#include "motor.h"
#include "dfplayer.h"
#include "keypad.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

typedef enum {
    STATE_NORMAL,
    STATE_INTERACTIVE
} SystemState;

static uint8_t note_playing = 0;

// UART DEBUUUUUUUUUUUUUUUUUUUUUUG
void debug_uart_init(void) {
    UBRR0 = 103;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (3 << UCSZ00); 
}

void debug_uart_write(uint8_t data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

void debug_uart_string(const char* str) {
    while (*str) {
        debug_uart_write(*str++);
    }
}

void debug_uart_number(uint16_t val) {
    char buf[6];
    int8_t i = 0;
    if (val == 0) {
        debug_uart_write('0');
        return;
    }
    while (val > 0) {
        buf[i++] = (val % 10) + '0';
        val /= 10;
    }
    while (--i >= 0) {
        debug_uart_write(buf[i]);
    }
}

void debug_print_sensor(uint8_t sensor_idx, uint16_t val) {
    debug_uart_string("S");
    debug_uart_write('0' + sensor_idx);
    debug_uart_string(" = ");
    debug_uart_number(val);
    debug_uart_string("\r\n");
}

void gpio_init(void)
{
    MUX_DDR |= (1 << MUX_S0) | (1 << MUX_S1) | (1 << MUX_S2);

    MOTOR_DDR |= (1 << MOTOR_PIN);
    MOTOR_PORT &= ~(1 << MOTOR_PIN);

    BUZZER_DDR |= (1 << BUZZER_PIN);
    BUZZER_PORT &= ~(1 << BUZZER_PIN);

    DFP_TX_DDR |= (1 << DFP_TX_PIN);
    DFP_TX_PORT |= (1 << DFP_TX_PIN);

    DFP_RX_DDR &= ~(1 << DFP_RX_PIN);
    DFP_RX_PORT |= (1 << DFP_RX_PIN);

    KBD_ROW_DDR |= (1 << KBD_ROW1) | (1 << KBD_ROW2) | (1 << KBD_ROW3);
    KBD_ROW4_DDR |= (1 << KBD_ROW4);
    
    KBD_ROW_PORT |= (1 << KBD_ROW1) | (1 << KBD_ROW2) | (1 << KBD_ROW3);
    KBD_ROW4_PORT |= (1 << KBD_ROW4);

    KBD_COL_DDR &= ~((1 << KBD_COL1) | (1 << KBD_COL2) | (1 << KBD_COL3));
    KBD_COL_PORT |= (1 << KBD_COL1) | (1 << KBD_COL2) | (1 << KBD_COL3);
}

int main(void)
{
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    gpio_init();
    keypad_init();
    debug_uart_init();

    debug_uart_string("\r\n=== MESA INTERACTIVA STARTUP ===\r\n");

    for (uint8_t i = 0; i < 3; i++) {
        motor_on();
        _delay_ms(150);
        motor_off();
        _delay_ms(150);
    }

    adc_init();
    timer2_init();
    buzzer_init();      
    mux_init();
    dfplayer_init();    

    for (uint8_t i = 0; i < 2; i++) {
        motor_on();
        _delay_ms(150);
        motor_off();
        _delay_ms(150);
    }

    sei();
    debug_uart_string("=== SISTEMA LISTO Y ESCANEANDO ===\r\n");

    SystemState state = STATE_NORMAL;
    uint32_t last_detect_ms = 0;
    uint8_t active_sensor = 0xFF;
    uint32_t last_debug_ms = 0;
    uint8_t current_volume = 15;

    dfplayer_play_folder(1, 1);

    while (1)
    {
        IR_ScanResult scan = mux_scan_all();

        if ((millis() - last_debug_ms) >= 500) {
            last_debug_ms = millis();
            for (uint8_t i = 0; i < 8; i++) {
                debug_print_sensor(i, scan.value[i]);
            }
            debug_uart_string("----\r\n");
        }

        switch (state)
        {
            case STATE_NORMAL:
                motor_off();
                buzzer_stop();

                if (note_playing) {
                    dfplayer_resume();
                    note_playing = 0;
                }

                char key = keypad_get_pressed_key();
                if (key != 0) {
                    if (key >= '1' && key <= '9') {
                        uint8_t track = key - '0';
                        debug_uart_string("Keypad: Seleccionando pista ");
                        debug_uart_write(key);
                        debug_uart_string("\r\n");
                        dfplayer_play_folder(1, track);
                    }
                    else if (key == '*') {
                        if (current_volume >= 3) current_volume -= 3;
                        else current_volume = 0;
                        debug_uart_string("Keypad: Volumen - (");
                        debug_uart_number(current_volume);
                        debug_uart_string(")\r\n");
                        dfplayer_set_volume(current_volume);
                    }
                    else if (key == '#') {
                        if (current_volume <= 27) current_volume += 3;
                        else current_volume = 30;
                        debug_uart_string("Keypad: Volumen + (");
                        debug_uart_number(current_volume);
                        debug_uart_string(")\r\n");
                        dfplayer_set_volume(current_volume);
                    }
                }

                if (scan.any_active) {
                    active_sensor = scan.first_active;
                    state = STATE_INTERACTIVE;
                    note_playing = 0;
                }
                break;

            case STATE_INTERACTIVE:
                if (scan.any_active) {
                    if (scan.first_active != active_sensor) {
                        active_sensor = scan.first_active;
                        note_playing = 0;
                    }

                    if (!note_playing) {
                        dfplayer_pause();              
                        _delay_ms(100);                
                        dfplayer_play_sensor_note(active_sensor);
                        buzzer_play_sensor_note(active_sensor);
                        note_playing = 1;
                    }

                    motor_on();                        
                    last_detect_ms = millis();         
                }
                else {
                    if ((millis() - last_detect_ms) >= DEBOUNCE_EXIT_MS) {
                        active_sensor = 0xFF;
                        state = STATE_NORMAL;
                    }
                }
                break;

            default:
                state = STATE_NORMAL;
                break;
        }
    }

    return 0;
}