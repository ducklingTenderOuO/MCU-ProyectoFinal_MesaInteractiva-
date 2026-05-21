/*
 * timers.h
 *
 * Created: 18/05/2026 11:12:00 a. m.
 *  Author: astri
 */

#ifndef TIMERS_H
#define TIMERS_H

#include <avr/io.h>
#include <stdint.h>

void     timer2_init(void);
uint32_t millis(void);

#endif /* TIMERS_H */
