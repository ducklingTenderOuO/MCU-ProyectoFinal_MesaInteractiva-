/*
 * config.h
 *
 * Created: 18/05/2026 08:30:00 a. m.
 *  Author: astri
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <avr/io.h>

// SISTEMAAAAAAAAAAAAA
#define F_CPU            16000000UL
#define NUM_SENSORS      6
#define LDR_THRESHOLD    600
#define DEBOUNCE_EXIT_MS 150

// MOTOR DCCCCCCCCCCCC
#define MOTOR_PORT   PORTB
#define MOTOR_DDR    DDRB
#define MOTOR_PIN    PB3 // Arduino Pin 11

// TECLADO MATRICIALLLLLLLL
// Filas en D2, D3, D4, D5
#define KBD_ROW_PORT   PORTD
#define KBD_ROW_DDR    DDRD
#define KBD_ROW1       PD2
#define KBD_ROW2       PD3
#define KBD_ROW3       PD4
#define KBD_ROW4       PD5

// Columnas en D6, D7, D8, D9
#define KBD_COL_PORT   PORTD
#define KBD_COL_DDR    DDRD
#define KBD_COL_PIN    PIND
#define KBD_COL1       PD6
#define KBD_COL2       PD7

#define KBD_COL_B_PORT PORTB
#define KBD_COL_B_DDR  DDRB
#define KBD_COL_B_PIN  PINB
#define KBD_COL3       PB0 // Pin 8
#define KBD_COL4       PB1 // Pin 9

// DFPLAYER MINIIIIIIIIIII
#define DFP_TX_DDR   DDRB
#define DFP_TX_PORT  PORTB
#define DFP_TX_PIN   PB4 // Pin 12

#define DFP_RX_DDR   DDRB
#define DFP_RX_PORT  PORTB
#define DFP_RX_PIN   PB5 // Pin 13

#endif /* CONFIG_H */
