/*
 * dfplayer.c
 *
 * Created: 20/05/2026 03:22:01 p. m.
 *  Author: astri
 */

#include "dfplayer.h"
#include "config.h"
#include <avr/interrupt.h>

static void precise_delay_ms(uint16_t ms)
{
    while (ms--) {
        __asm__ __volatile__ (
            "ldi r24, lo8(4000)\n\t"
            "ldi r25, hi8(4000)\n\t"
            "1: sbiw r24, 1\n\t"
            "brne 1b\n\t"
            :
            :
            : "r24", "r25"
        );
    }
}

static inline void delay_104us(void)
{
    __asm__ __volatile__ (
        "ldi r24, lo8(416)\n\t"
        "ldi r25, hi8(416)\n\t"
        "1: sbiw r24, 1\n\t"
        "brne 1b\n\t"
        :
        :
        : "r24", "r25"
    );
}

static void dfplayer_uart_tx_byte(uint8_t byte)
{
    uint8_t sreg = SREG;
    cli();

    DFP_TX_PORT &= ~(1 << DFP_TX_PIN);
    delay_104us();

    for (uint8_t i = 0; i < 8; i++) {
        if (byte & 0x01) {
            DFP_TX_PORT |= (1 << DFP_TX_PIN);
        } else {
            DFP_TX_PORT &= ~(1 << DFP_TX_PIN);
        }
        byte >>= 1;
        delay_104us();
    }

    DFP_TX_PORT |= (1 << DFP_TX_PIN);
    delay_104us();

    SREG = sreg;
}

static uint16_t dfplayer_checksum(uint8_t *buf)
{
    uint16_t sum = 0;
    for (uint8_t i = 1; i <= 6; i++) {
        sum += buf[i];
    }
    return (uint16_t)(0 - sum);
}

static void dfplayer_send_command(uint8_t cmd, uint16_t param)
{
    uint8_t buf[10];
    uint16_t checksum;

    buf[0] = 0x7E;
    buf[1] = 0xFF;
    buf[2] = 0x06;
    buf[3] = cmd;
    buf[4] = 0x00;
    buf[5] = (uint8_t)(param >> 8);
    buf[6] = (uint8_t)(param & 0xFF);

    checksum = dfplayer_checksum(buf);
    buf[7] = (uint8_t)(checksum >> 8);
    buf[8] = (uint8_t)(checksum & 0xFF);
    buf[9] = 0xEF;

    for (uint8_t i = 0; i < 10; i++) {
        dfplayer_uart_tx_byte(buf[i]);
    }

    precise_delay_ms(50);
}

void dfplayer_init(void)
{
    DFP_TX_DDR |= (1 << DFP_TX_PIN);
    DFP_TX_PORT |= (1 << DFP_TX_PIN);

    DFP_RX_DDR &= ~(1 << DFP_RX_PIN);
    DFP_RX_PORT |= (1 << DFP_RX_PIN);

    precise_delay_ms(500);
    dfplayer_reset();
    precise_delay_ms(500);

    dfplayer_set_volume(15);
}

void dfplayer_play_folder(uint8_t folder, uint8_t file)
{
    uint16_t param = ((uint16_t)folder << 8) | file;
    dfplayer_send_command(0x0F, param);
}

void dfplayer_pause(void)
{
    dfplayer_send_command(0x0E, 0x0000);
}

void dfplayer_resume(void)
{
    dfplayer_send_command(0x0D, 0x0000);
}

void dfplayer_set_volume(uint8_t volume)
{
    if (volume > 30) volume = 30;
    dfplayer_send_command(0x06, (uint16_t)volume);
}

void dfplayer_reset(void)
{
    dfplayer_send_command(0x0C, 0x0000);
}

void dfplayer_play_sensor_note(uint8_t sensor_index)
{
    if (sensor_index < 8) {
        dfplayer_play_folder(2, sensor_index + 1);
    }
}

void dfplayer_debug_print_adc(uint8_t sensor_idx, uint16_t val)
{
    dfplayer_uart_tx_byte('S');
    dfplayer_uart_tx_byte('0' + sensor_idx);
    dfplayer_uart_tx_byte(':');
    dfplayer_uart_tx_byte(' ');

    char buf[5];
    int8_t i = 0;
    if (val == 0) {
        dfplayer_uart_tx_byte('0');
    } else {
        uint16_t temp = val;
        while (temp > 0) {
            buf[i++] = (temp % 10) + '0';
            temp /= 10;
        }
        while (--i >= 0) {
            dfplayer_uart_tx_byte(buf[i]);
        }
    }
    dfplayer_uart_tx_byte('\r');
    dfplayer_uart_tx_byte('\n');
}