/*
 * mux.h
 *
 * Created: 19/05/2026 02:28:40 p. m.
 *  Author: astri
 */

#ifndef MUX_H
#define MUX_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint16_t value[8];
    bool     detected[8];
    bool     any_active;
    uint8_t  first_active;
} IR_ScanResult;

void          mux_init(void);
void          mux_select_channel(uint8_t channel);
uint16_t      mux_read_channel(uint8_t channel);
IR_ScanResult mux_scan_all(void);

#endif /* MUX_H */
