#include "ball_graphic.h"

bool blue_pacman_start = 0, green_pacman_start = 0;
ball_s ball_pos_direction;
bool ball_in_green_half;
bool ball_in_blue_half;

void increase_ball_x(ball_s *ball) {
  ball->vx = (ball->vx == 4) ? ball->vx : (ball->vx + 1);
  ball->vy = (ball->vy == 1) ? ball->vy : (ball->vy - 1);
}

void decrease_ball_x(ball_s *ball) {
  ball->vx = (ball->vx == 1) ? ball->vx : (ball->vx - 1);
  ball->vy = (ball->vy == 4) ? ball->vy : (ball->vy + 1);
}

void ball_setup(ball_s *ball) {
  ball->row = matrix_width / 2;
  ball->col = matrix_height / 2;
  ball->vx = 1;
  ball->vy = 1;
  ball->xDir = rand() % 2 ? 1 : -1;
  ball->yDir = rand() % 2 ? 1 : -1;
}

// Blue pacman functions
void set_blue_pacman_start() { blue_pacman_start = 1; }

void clear_blue_pacman_start() { blue_pacman_start = 0; }

bool get_blue_pacman_start() { return blue_pacman_start; }

// Green pacman functions
void set_green_pacman_start() { green_pacman_start = 1; }

void clear_green_pacman_start() { green_pacman_start = 0; }

bool get_green_pacman_start() { return green_pacman_start; }

// Ball position set and get functions
void set_ball_position_direction(ball_s *ball) { ball_pos_direction = *(ball_s *)ball; }

ball_s get_ball_position_direction() { return ball_pos_direction; }

void set_ball_in_green_half(bool in_green_half) {
  ball_in_green_half = in_green_half;
  ball_in_blue_half = !in_green_half;
}

void set_ball_in_blue_half(bool in_blue_half) {
  ball_in_blue_half = in_blue_half;
  ball_in_green_half = !in_blue_half;
}

bool isBallInGreenHalf() { return ball_in_green_half; }

bool isBallinBlueHalf() { return ball_in_blue_half; }