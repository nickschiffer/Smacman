#pragma once
#include "controller_comm.h"
#include "game_graphics.h"
#include "led_graphics.h"
#include "led_matrix.h"
#include "queue.h"
#include "score_graphics.h"
void ball_task(void *params);

typedef struct {
  uint8_t level;
  QueueHandle_t *state_queue;
} ball_param;
