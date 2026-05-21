/*
 * UART.h
 *
 * Created: 18/05/2026 09:42:00 a. m.
 *  Author: astri
 */

#ifndef UART_H_
#define UART_H_

#include <avr/io.h>

#define BAUD_9600     103
#define BAUD_19200     51
#define BAUD_38400     25
#define BAUD_115200     8

#define DATA_5   0b00
#define DATA_6   0b01
#define DATA_7   0b10
#define DATA_8   0b11

#define PARITY_NONE  0b00
#define PARITY_EVEN  0b10
#define PARITY_ODD   0b11

#define STOP_1  0
#define STOP_2  1

void          UART_init(unsigned int baud, unsigned char data,
                        unsigned char parity, unsigned char stop);
unsigned char UART_read(void);
void          UART_write(unsigned char data);
void          UART_string(char* string);

void UART_init(unsigned int baud, unsigned char data,
               unsigned char parity, unsigned char stop)
{
    UBRR0 = baud;
    UCSR0B = (1<<RXCIE0) | (1<<RXEN0) | (1<<TXEN0);
    UCSR0C = (parity << UPM00) | (stop << USBS0) | (data << UCSZ00);
}

unsigned char UART_read(void)
{
    return UDR0;
}

void UART_write(unsigned char data)
{
    while(!(UCSR0A & (1<<UDRE0)));
    UDR0 = data;
}

void UART_string(char* string)
{
    while(*string != 0)
    {
        UART_write(*string);
        string++;
    }
}

#endif /* UART_H_ */
