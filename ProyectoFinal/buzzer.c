/*
 * buzzer.c
 *
 * Created: 19/05/2026 11:42:30 a. m.
 *  Author: astri
 */

#include "buzzer.h"
#include "config.h"
#include <avr/io.h>

static const uint16_t note_freq[8] = {
    262, // Do4
    294, // Re4
    330, // Mi4
    349, // Fa4
    392, // Sol4
    440, // La4
    494, // Si4
    523  // Do5
};

void buzzer_init(void)
{
    TCCR1A = 0;
    TCCR1B = (1 << WGM12);
    TCCR1C = 0;
    OCR1A  = 0;
    TIMSK1 = 0;
    TCNT1  = 0;
}

void buzzer_play_note(uint16_t frequency_hz)
{
    if (frequency_hz == 0) {
        buzzer_stop();
        return;
    }

    uint16_t ocr = (uint16_t)((F_CPU / (16UL * frequency_hz)) - 1);

    OCR1A  = ocr;
    TCNT1  = 0;

    TCCR1A = (1 << COM1A0);
    TCCR1B = (1 << WGM12) | (1 << CS11);
}

void buzzer_stop(void)
{
    TCCR1A = 0;
    TCCR1B = (1 << WGM12);
    BUZZER_PORT &= ~(1 << BUZZER_PIN);
}

void buzzer_play_sensor_note(uint8_t sensor_index)
{
    if (sensor_index < 8) {
        buzzer_play_note(note_freq[sensor_index]);
    }
}
