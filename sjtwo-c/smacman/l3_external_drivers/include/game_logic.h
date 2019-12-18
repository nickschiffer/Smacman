#pragma once
#include "ball_graphic.h"
#include "game_graphics.h"
#include "pacman.h"
#include "paddle.h"
#include "score_graphics.h"

typedef enum { blue_pacman, green_pacman, paddle_blue, paddle_green, ball, players_score } game_logic_task_list_s;
typedef enum {
  blue_pacman_l1,
  blue_pacman_l2,
  blue_pacman_l3,
  green_pacman_l1,
  green_pacman_l2,
  green_pacman_l3
} game_logic_pacman_levels_s;
typedef enum { TASK_NOT_SUSPENDED, TASK_SUSPENDED = 1 } game_logic_task_state_s;

game_logic_game_state_s get_game_state(void);
void set_game_state(game_logic_game_state_s game_state);