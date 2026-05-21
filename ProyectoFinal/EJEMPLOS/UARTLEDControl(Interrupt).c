/*
 * UARTLEDControl(Interrupt).c
 *
 * Created: 18/05/2026 09:45:00 a. m.
 *  Author: astri
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "UART.h"
#include "EEPROM.h"

#define EEPROM_ADDR  0x00

volatile bool    RxFlag = false;
volatile uint8_t RxData = 0;

void apply_leds(uint8_t state);
void print_state(uint8_t state);

void apply_leds(uint8_t state)
{
    PORTB = (PORTB & 0xC0) | (state & 0x3F);
    PORTC = (PORTC & 0xFC) | ((state >> 6) & 0x03);
}

void print_state(uint8_t state)
{
    for(int8_t i = 7; i >= 0; i--)
    {
        UART_write((state & (1<<i)) ? '1' : '0');
    }
    UART_string("\n\r");
}

int main(void)
{
    DDRB = 0x3F;
    DDRC = (DDRC & 0xFC) | 0x03;

    UART_init(BAUD_9600, DATA_8, PARITY_NONE, STOP_1);

    uint8_t led_state = EEPROM_read(EEPROM_ADDR);
    if(led_state == 0xFF) led_state = 0x00;
    apply_leds(led_state);

    sei();

    UART_string("=== Control 8 LEDs ===\n\r");
    UART_string("ON : 1 2 3 4 5 6 7 8\n\r");
    UART_string("OFF: Q W E R T Y U I\n\r");
    UART_string("Estado: ");
    print_state(led_state);

    while(1)
    {
        if(RxFlag)
        {
            RxFlag = false;
            uint8_t cmd = RxData;

            if(cmd >= '1' && cmd <= '8')
            {
                uint8_t bit = cmd - '1';
                led_state |= (1 << bit);
                apply_leds(led_state);
                EEPROM_update(EEPROM_ADDR, led_state);
                UART_string("LED");
                UART_write(cmd);
                UART_string(" ON  | ");
                print_state(led_state);
            }
            else
            {
                int8_t bit = -1;
                switch(cmd)
                {
                    case 'Q': case 'q':  bit = 0; break;
                    case 'W': case 'w':  bit = 1; break;
                    case 'E': case 'e':  bit = 2; break;
                    case 'R': case 'r':  bit = 3; break;
                    case 'T': case 't':  bit = 4; break;
                    case 'Y': case 'y':  bit = 5; break;
                    case 'U': case 'u':  bit = 6; break;
                    case 'I': case 'i':  bit = 7; break;
                    default:             bit = -1; break;
                }

                if(bit >= 0)
                {
                    led_state &= ~(1 << bit);
                    apply_leds(led_state);
                    EEPROM_update(EEPROM_ADDR, led_state);
                    UART_string("LED");
                    UART_write('1' + bit);
                    UART_string(" OFF | ");
                    print_state(led_state);
                }
                else
                {
                    UART_string("INVALID\n\r");
                }
            }
        }
    }
}

ISR(USART_RX_vect)
{
    RxData = UDR0;
    RxFlag = true;
}
