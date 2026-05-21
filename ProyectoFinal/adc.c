/*
 * adc.c
 *
 * Created: 18/05/2026 09:15:20 a. m.
 *  Author: astri
 */

#include "adc.h"

void adc_init(void)
{
    ADMUX = (1 << REFS0);

    ADCSRA = (1 << ADEN)  |
             (1 << ADPS2) |
             (1 << ADPS1) |
             (1 << ADPS0);

    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
}

uint16_t adc_read(uint8_t channel)
{
    ADMUX = (1 << REFS0) | (channel & 0x07);

    ADCSRA |= (1 << ADSC);

    while (ADCSRA & (1 << ADSC));

    return ADC;
}
