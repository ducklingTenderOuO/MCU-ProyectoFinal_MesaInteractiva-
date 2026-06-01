/*
 * main.c
 *
 * Created: 20/05/2026 05:45:30 p. m.
 *  Author: astri
 */

#include "config.h"
#include "adc.h"
#include "timers.h"
#include "motor.h"
#include "dfplayer.h"
#include "keypad.h"

typedef struct {
    uint8_t any_active;
    uint8_t first_active;
    uint16_t value[NUM_SENSORS];
    uint8_t detected[NUM_SENSORS];
} LDR_ScanResult;

LDR_ScanResult ldr_scan_all(void) {
    LDR_ScanResult result;
    result.any_active = 0;
    result.first_active = 0xFF;

    for (uint8_t i = 0; i < NUM_SENSORS; i++) {
        result.value[i] = adc_read(i);
        result.detected[i] = (result.value[i] > LDR_THRESHOLD);

        if (result.detected[i] && !result.any_active) {
            result.any_active = 1;
            result.first_active = i;
        }
    }

    return result;
}
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
    MOTOR_DDR |= (1 << MOTOR_PIN);
    MOTOR_PORT &= ~(1 << MOTOR_PIN); // Active High

    DFP_TX_DDR |= (1 << DFP_TX_PIN);
    DFP_TX_PORT |= (1 << DFP_TX_PIN);

    DFP_RX_DDR &= ~(1 << DFP_RX_PIN);
    DFP_RX_PORT |= (1 << DFP_RX_PIN);
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

    dfplayer_loop_folder(1);

    while (1)
    {
        LDR_ScanResult scan = ldr_scan_all();

        if ((millis() - last_debug_ms) >= 500) {
            last_debug_ms = millis();
            for (uint8_t i = 0; i < NUM_SENSORS; i++) {
                debug_print_sensor(i, scan.value[i]);
            }
            debug_uart_string("----\r\n");
        }

        switch (state)
        {
            case STATE_NORMAL:
                motor_off();

                if (note_playing) {
                    dfplayer_loop_folder(1); // Reproducir en bucle la carpeta 1 (automático)
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
                        debug_uart_string("Keypad: Volumen -\r\n");
                        dfplayer_volume_down();
                    }
                    else if (key == '#') {
                        debug_uart_string("Keypad: Volumen +\r\n");
                        dfplayer_volume_up();
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
                        dfplayer_play_sensor_note(active_sensor);
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