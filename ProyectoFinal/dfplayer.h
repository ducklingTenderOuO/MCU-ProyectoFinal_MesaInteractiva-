/*
 * dfplayer.h
 *
 * Created: 20/05/2026 03:21:44 p. m.
 *  Author: astri
 */

#ifndef DFPLAYER_H
#define DFPLAYER_H

#include <stdint.h>

void dfplayer_init(void);
void dfplayer_play_folder(uint8_t folder, uint8_t file);
void dfplayer_pause(void);
void dfplayer_resume(void);
void dfplayer_set_volume(uint8_t volume);
void dfplayer_reset(void);
void dfplayer_play_sensor_note(uint8_t sensor_index);
void dfplayer_debug_print_adc(uint8_t sensor_idx, uint16_t val);

#endif /* DFPLAYER_H */