/*
 * motor.c
 *
 * Created: 18/05/2026 10:20:00 a. m.
 *  Author: astri
 */

#include "motor.h"
#include "config.h"
#include <avr/io.h>

void motor_on(void)
{
    MOTOR_PORT |= (1 << MOTOR_PIN);
}

void motor_off(void)
{
    MOTOR_PORT &= ~(1 << MOTOR_PIN);
}
