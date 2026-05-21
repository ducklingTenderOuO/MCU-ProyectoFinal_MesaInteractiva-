/*
 * buzzer.h
 *
 * Created: 19/05/2026 11:39:15 a. m.
 *  Author: astri
 */

#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

void buzzer_init(void);
void buzzer_play_note(uint16_t frequency_hz);
void buzzer_stop(void);
void buzzer_play_sensor_note(uint8_t sensor_index);

#endif /* BUZZER_H */
