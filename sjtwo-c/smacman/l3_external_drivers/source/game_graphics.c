#include "game_graphics.h"

void increase_ball_x(ball_s *ball) {
  ball->vx = (ball->vx == 4) ? ball->vx : (ball->vx + 1);
  ball->vy = (ball->vy == 1) ? ball->vy : (ball->vy - 1);
}

void decrease_ball_x(ball_s *ball) {
  ball->vx = (ball->vx == 1) ? ball->vx : (ball->vx - 1);
  ball->vy = (ball->vy == 4) ? ball->vy : (ball->vy + 1);
}

void clear_ball(int row, int column) {
  led_matrix__clear_pixel(row, column);
  led_matrix__clear_pixel(row, column - 1);
  led_matrix__clear_pixel(row, column + 1);
  led_matrix__clear_pixel(row, column - 2);
  led_matrix__clear_pixel(row, column + 2);

  led_matrix__clear_pixel(row - 1, column);
  led_matrix__clear_pixel(row - 1, column - 1);
  led_matrix__clear_pixel(row - 1, column + 1);
  led_matrix__clear_pixel(row - 1, column - 2);
  led_matrix__clear_pixel(row - 1, column + 2);

  led_matrix__clear_pixel(row + 1, column);
  led_matrix__clear_pixel(row + 1, column - 1);
  led_matrix__clear_pixel(row + 1, column + 1);
  led_matrix__clear_pixel(row + 1, column - 2);
  led_matrix__clear_pixel(row + 1, column + 2);

  led_matrix__clear_pixel(row - 2, column);
  led_matrix__clear_pixel(row - 2, column - 1);
  led_matrix__clear_pixel(row - 2, column + 1);

  led_matrix__clear_pixel(row + 2, column);
  led_matrix__clear_pixel(row + 2, column - 1);
  led_matrix__clear_pixel(row + 2, column + 1);
}

void ball_setup(ball_s *ball) {
  ball->row = matrix_width / 2;
  ball->col = matrix_height / 2;
  ball->vx = 1;
  ball->vy = 1;
  ball->xDir = rand() % 2 ? 1 : -1;
  ball->yDir = rand() % 2 ? 1 : -1;
}

void set_pacman_row_and_col(int row, int col, game_graphics_players which_player) {
  //   printf("Player_%i_row = %i, Player_%i_col = %i\n", which_player, row, which_player, col);
  switch (which_player) {
  case 1:
    player_1_packman_col = col;
    player_1_packman_row = row;
    break;
  case 2:
  default:
    player_2_packman_col = col;
    player_2_packman_row = row;
    break;
  }
}

volatile bool collided = 0;
void detect_collision(ball_s ball) {
  //   printf("Ball_row = %i, Ball_col = %i\n", ball.row, ball.col);
  if ((ball.row >= player_1_packman_row && ball.row <= (player_1_packman_row + 5)) &&
      (ball.col >= player_1_packman_col && ball.col <= (player_1_packman_col + 8)) && collided == 0) {
    printf("Player 1 Collision Detected\n");
    collided = 1;
    return;
  } else if ((ball.row >= player_2_packman_row && ball.row <= (player_2_packman_row + 5)) &&
             (ball.col >= player_2_packman_col && ball.col <= (player_2_packman_col + 8)) && collided == 0) {
    printf("Player 2 Collision Detected\n");
    collided = 1;
    return;
  }
  led_matrix__drawBall(ball.row, ball.col, RED);
}

void blue_pacman_setup(pacman_s *blue_pacman) {
  blue_pacman->row_upordown_right = 5;
  blue_pacman->row_upordown_left = matrix_width - 6;
  blue_pacman->row_left_upordown = 2;
  blue_pacman->row_right_upordown = matrix_width - 3;
  blue_pacman->col_up_leftorright = (matrix_height / 2);
  blue_pacman->col_down_leftorright = matrix_height - 5;
  blue_pacman->col_leftorrigt_up = matrix_height - 8;
  blue_pacman->col_leftorrigt_down = (matrix_height / 2) + 3;
  blue_pacman->packman_color = BLUE;
  blue_pacman->direction = LEFT_UP;
}

void green_pacman_setup(void) {}

void game_graphics_packman(int row, int column, led_matrix__direction_e direction, led_matrix__color_e packman_color,
                           game_graphics_players which_player) {

  led_matrix__drawPackMan(row, column, direction, packman_color);
  set_pacman_row_and_col(row, column, which_player);
}