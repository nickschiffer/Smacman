#pragma once

#include "common.h"

#define VS1053_FILEPLAYER_PIN_INT 5

#define VS1053_SCI_READ 0x03
#define VS1053_SCI_WRITE 0x02

#define VS1053_REG_MODE 0x00
#define VS1053_REG_STATUS 0x01
#define VS1053_REG_BASS 0x02
#define VS1053_REG_CLOCKF 0x03
#define VS1053_REG_DECODETIME 0x04
#define VS1053_REG_AUDATA 0x05
#define VS1053_REG_WRAM 0x06
#define VS1053_REG_WRAMADDR 0x07
#define VS1053_REG_HDAT0 0x08
#define VS1053_REG_HDAT1 0x09
#define VS1053_REG_VOLUME 0x0B

#define VS1053_MODE_SM_DIFF 0x0001
#define VS1053_MODE_SM_LAYER12 0x0002
#define VS1053_MODE_SM_RESET 0x0004
#define VS1053_MODE_SM_CANCEL 0x0008
#define VS1053_MODE_SM_EARSPKLO 0x0010
#define VS1053_MODE_SM_TESTS 0x0020
#define VS1053_MODE_SM_STREAM 0x0040
#define VS1053_MODE_SM_SDINEW 0x0800
#define VS1053_MODE_SM_ADPCM 0x1000
#define VS1053_MODE_SM_LINE1 0x4000
#define VS1053_MODE_SM_CLKRANGE 0x8000

void mp3__set_xdcs_low();
void mp3__set_xdcs_high();
void mp3__set_dcs(bool s);
bool mp3__ready_for_data(void);
// void mp3__wait_for_dreq(void);
void mp3__sci_write(uint8_t addr, uint16_t data);
uint16_t mp3__sci_read(uint8_t addr);
void mp3__reset(void);
void mp3__set_volume(uint8_t left, uint8_t right);
void mp3__set_bass_treble(uint8_t b, uint8_t t);
void mp3__init_song();
void mp3__send_data(uint8_t *buffer, uint16_t buffsize);
void mp3__sine_test(uint8_t n, uint16_t ms);
bool mp3__init();

void mp3__play_track_task(void *songQueueHandle);
void mp3__play_sounds_task(void *xGameSoundsQueueHandle);