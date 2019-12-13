#include "common.h"

void common__queue_init(xQueueHandle *xSongQueue, xQueueHandle *xGameSoundsQueue) {
  *xSongQueue = xQueueCreate(1, VS1053_DATABUFFERLEN);
  *xGameSoundsQueue = xQueueCreate(1, sizeof(uint8_t));
}

bool common__init(xQueueHandle *xSongQueue, xQueueHandle *xGameSoundsQueue) {
  common__queue_init(xSongQueue, xGameSoundsQueue);
}
