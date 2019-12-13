#pragma once
#include "led_graphics.h"
#include "led_matrix.h"

typedef struct {
  int row_start;
  int col;
  led_matrix__color_e paddle_color;
} paddle_s;

void paddle_task(void *params);