#pragma once

#include "FreeRTOS.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "delay.h"
#include "led_matrix.h"
#include "queue.h"
#include "sdcard_handle.h"
#include "ssp_driver.h"

#define VS1053_DATABUFFERLEN 512

bool common__init(xQueueHandle *xSongQueue, xQueueHandle *xGameSoundsQueue);
