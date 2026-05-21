/*
 * adc.h
 *
 * Created: 18/05/2026 09:12:05 a. m.
 *  Author: astri
 */

#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include <stdint.h>

void     adc_init(void);
uint16_t adc_read(uint8_t channel);

#endif /* ADC_H */
