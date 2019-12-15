#include "game_graphics.h"

player player1, player2;
bool blue_collided = 0, green_collided = 0;
bool start_detection = 0;

void set_pacman_row_and_col(int row, int col, game_graphics_players which_player) {
  //   SMACMAN__DEBUG_PRINTF("Player_%i_row = %i, Player_%i_col = %i\n", which_player, row, which_player, col);
  switch (which_player) {
  case 1:
    player1.col = col;
    player1.row = row;
    break;
  case 2:
  default:
    player2.col = col;
    player2.row = row;
    break;
  }
}

bool set_blue_collided() {
  blue_collided = true;
  return blue_collided;
}

bool clear_blue_collided() {
  blue_collided = false;
  return blue_collided;
}
bool get_blue_collided() { return blue_collided; }

bool set_green_collided() {
  green_collided = true;
  return green_collided;
}

bool clear_green_collided() {
  green_collided = false;
  return green_collided;
}
bool get_green_collided() { return green_collided; }

bool set_start_detection() {
  start_detection = true;
  return start_detection;
}
bool get_start_detection() { return start_detection; }
bool clear_start_detection() {
  start_detection = false;
  return start_detection;
}

void detect_collision(ball_s ball) {
  if ((ball.row >= (player1.row - 4) && ball.row <= (player1.row + 4)) &&
      (ball.col >= (player1.col - 4) && ball.col <= (player1.col + 4)) && (start_detection == 1)) {
    // if (ball.col > matrix_width / 2 + 2 && ball.xDir < 0 && ball.col < matrix_width - 5) {
    if (ball.yDir > 0) {
      // printf("blue hit:%i\n", ball.yDir);
      SMACMAN__DEBUG_PRINTF("Player 1 Collision Detected\n");
      blue_collided = set_blue_collided();
    }
    return;
  } else if ((ball.row >= (player2.row - 4) && ball.row <= (player2.row + 4)) &&
             (ball.col >= (player2.col - 4) && ball.col <= (player2.col + 4)) && (start_detection == 1)) {
    // if (ball.col < matrix_width / 2 - 2 && ball.xDir > 0 && ball.col > 4) {
    if (ball.yDir < 0) {
      // printf("green hit:%i\n", ball.yDir);
      SMACMAN__DEBUG_PRINTF("Player 2 Collision Detected\n");
      green_collided = set_green_collided();
    }
    return;
  }

  led_matrix__drawBall(ball.row, ball.col, RED);
}

void game_graphics_packman(int row, int column, led_matrix__direction_e direction, led_matrix__color_e packman_color,
                           game_graphics_players which_player) {

  led_matrix__drawPackMan(row, column, direction, packman_color);
  set_pacman_row_and_col(row, column, which_player);
}
