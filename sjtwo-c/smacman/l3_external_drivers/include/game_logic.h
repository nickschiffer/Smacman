#pragma once
#include "game_graphics.h"

typedef enum { INIT_STATE = 0, IN_PROGRESS_STATE = 1 } game_logic_game_state_s;

typedef enum { blue_pacman, green_pacman, paddle_blue, paddle_green, ball } game_logic_task_list_s;
typedef enum { TASK_NOT_SUSPENDED, TASK_SUSPENDED = 1 } game_logic_task_state_s;
