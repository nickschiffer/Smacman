#pragma once
#include "common.h"

#include "ff.h"

bool sdcard_handle__init();
void sdcard_handle__read_song_task(void *songQueueHandle);