#pragma once

#include "FreeRTOS.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ff.h"
#include "led_matrix.h"
#include "ssp2.h"

#define DATA_SIZE 512
#define MP3_PROCESSING_SIZE 32
#define SCI_WRITE_OPCODE 0x02
#define SCI_CLK_REG 0x03
#define SCI_VOL_REG 0x0B
#define SCI_READ_OPCODE 0x03

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

void mp3_pins_init();
bool file_init();
void wait_for_dreq(void);
void write_to_sci_reg(char addr, char msb, char lsb);
void send_sdi_byte(uint8_t byte);
void set_xdcs_low();
void set_xdcs_high();
bool mp3_init();
bool mp3_read_from_sd();
bool open_file(char *filepath, FIL *fp);
bool read_data_from_sd_and_put_in_queue(FIL *fp);
void mp3_play_song_from_queue();
void read_dir();
uint16_t sci_read(uint8_t addr);