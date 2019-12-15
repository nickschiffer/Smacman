#pragma once
#include "FreeRTOS.h"
#include "led_graphics.h"

typedef struct {
  int row;
  int col;
} player;

typedef struct {
  int8_t row;
  int8_t col;
  uint8_t vx;
  uint8_t vy;
  int8_t xDir;
  int8_t yDir;
} ball_s;

typedef enum { PLAYER_1 = 1, PLAYER_2 } game_graphics_players;

bool set_blue_collided(void);
bool clear_blue_collided(void);
bool get_blue_collided(void);

bool set_green_collided(void);
bool clear_green_collided(void);
bool get_green_collided(void);

bool set_start_detection(void);
bool get_start_detection(void);
bool clear_start_detection(void);

void set_pacman_row_and_col(int row, int col, game_graphics_players which_player);

void detect_collision(ball_s ball);

void game_graphics_packman(int row, int column, led_matrix__direction_e direction, led_matrix__color_e packman_color,
                           game_graphics_players which_player);