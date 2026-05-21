/*
 * timers.c
 *
 * Created: 18/05/2026 11:15:00 a. m.
 *  Author: astri
 */

#include "timers.h"
#include <avr/interrupt.h>

static volatile uint32_t _ticks = 0;

void timer2_init(void)
{
    TCCR2A = (1 << WGM21);
    TCCR2B = (1 << CS22);
    OCR2A = 249;
    TIMSK2 = (1 << OCIE2A);
}

ISR(TIMER2_COMPA_vect)
{
    _ticks++;
}

uint32_t millis(void)
{
    uint32_t t;
    uint8_t sreg = SREG;
    cli();
    t = _ticks;
    SREG = sreg;
    return t;
}
