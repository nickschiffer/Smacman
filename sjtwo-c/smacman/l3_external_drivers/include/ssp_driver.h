#pragma once

#include <stdbool.h>
#include <stdint.h>

#define SSP2__DMA_TX_CHANNEL 0
#define SSP2__DMA_RX_CHANNEL 1

void c_ssp2__init(uint32_t max_clock_mhz);

uint8_t c_ssp2__exchange_byte(uint8_t data_out);