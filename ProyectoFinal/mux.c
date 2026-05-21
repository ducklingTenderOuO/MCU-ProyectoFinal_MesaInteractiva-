/*
 * mux.c
 *
 * Created: 19/05/2026 02:30:15 p. m.
 *  Author: astri
 */

#include "mux.h"
#include "adc.h"
#include "config.h"
#include <avr/io.h>
#include <util/delay.h>

#define MUX_SETTLE_US 1

void mux_init(void)
{
    mux_select_channel(0);
}

void mux_select_channel(uint8_t channel)
{
    MUX_PORT &= ~((1 << MUX_S0) | (1 << MUX_S1) | (1 << MUX_S2));

    if (channel & 0x01) MUX_PORT |= (1 << MUX_S0);
    if (channel & 0x02) MUX_PORT |= (1 << MUX_S1);
    if (channel & 0x04) MUX_PORT |= (1 << MUX_S2);
}

uint16_t mux_read_channel(uint8_t channel)
{
    mux_select_channel(channel);
    _delay_us(MUX_SETTLE_US);
    return adc_read(MUX_ADC_CH);
}

IR_ScanResult mux_scan_all(void)
{
    IR_ScanResult result;
    uint8_t i;

    result.any_active   = false;
    result.first_active = 0xFF;

    for (i = 0; i < NUM_SENSORS; i++) {
        result.value[i] = mux_read_channel(i);
        result.detected[i] = (result.value[i] < IR_THRESHOLD);

        if (result.detected[i] && !result.any_active) {
            result.any_active   = true;
            result.first_active = i;
        }
    }

    return result;
}
