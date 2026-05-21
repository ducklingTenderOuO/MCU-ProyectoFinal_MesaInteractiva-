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
#define NUM_SENSORS      8
#define IR_THRESHOLD     512
#define DEBOUNCE_EXIT_MS 150

// MULTIPLEXOR CD4051111111
#define MUX_PORT     PORTC
#define MUX_DDR      DDRC
#define MUX_S0       PC1
#define MUX_S1       PC2
#define MUX_S2       PC3
#define MUX_ADC_CH   0

// MOTOR DCCCCCCCCCCCC
#define MOTOR_PORT   PORTC
#define MOTOR_DDR    DDRC
#define MOTOR_PIN    PC4

// TECLADO MATRICIALLLLLLLL
#define KBD_ROW_PORT   PORTD
#define KBD_ROW_DDR    DDRD
#define KBD_ROW1       PD5
#define KBD_ROW2       PD6
#define KBD_ROW3       PD7

#define KBD_ROW4_PORT  PORTB
#define KBD_ROW4_DDR   DDRB
#define KBD_ROW4       PB0

#define KBD_COL_PORT   PORTD
#define KBD_COL_DDR    DDRD
#define KBD_COL_PIN    PIND
#define KBD_COL1       PD2
#define KBD_COL2       PD3
#define KBD_COL3       PD4

// BUZZER PWMMMMMMMMMM
#define BUZZER_DDR   DDRB
#define BUZZER_PORT  PORTB
#define BUZZER_PIN   PB1

// DFPLAYER MINIIIIIIIIIII
#define DFP_TX_DDR   DDRB
#define DFP_TX_PORT  PORTB
#define DFP_TX_PIN   PB2

#define DFP_RX_DDR   DDRB
#define DFP_RX_PORT  PORTB
#define DFP_RX_PIN   PB3

#endif /* CONFIG_H */
