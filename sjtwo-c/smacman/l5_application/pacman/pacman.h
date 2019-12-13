#pragma once
#include "ball_graphic.h"
#include "game_graphics.h"
#include "led_graphics.h"
#include "led_matrix.h"

typedef struct {
  int row_upordown_right;
  int row_upordown_left;
  int row_left_upordown;
  int row_right_upordown;
  int col_up_leftorright;
  int col_down_leftorright;
  int col_leftorrigt_up;
  int col_leftorrigt_down;
  led_matrix__color_e packman_color;
  led_matrix__direction_e direction;
  int level;
} pacman_s;

void pacman_task_level1(void *task_param);
void pacman_task_level2(void *task_param);
void pacman_task_level3(void *task_param);

// void blue_pacman_task(void *params);
// void green_pacman_task(void *params);
