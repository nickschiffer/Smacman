#include "pacman_graphics.h"

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

void green_pacman_setup(pacman_s *green_pacman) {
  green_pacman->row_upordown_right = 5;
  green_pacman->row_upordown_left = matrix_width - 6;
  green_pacman->row_left_upordown = 2;
  green_pacman->row_right_upordown = matrix_width - 3;
  green_pacman->col_up_leftorright = 4;
  green_pacman->col_down_leftorright = (matrix_height / 2) - 1;
  green_pacman->col_leftorrigt_up = (matrix_height / 2) - 4;
  green_pacman->col_leftorrigt_down = 7;
  green_pacman->packman_color = GREEN;
  green_pacman->direction = RIGHT_DOWN;
}

bool pac_detection(pacman_s *pacman) {
  bool blue_pacman_start, green_pacman_start;
  ball_s ball_pos;
  if (pacman->packman_color == BLUE) {
    ball_pos = get_ball_position_direction();
    if (ball_pos.row > matrix_width / 2 && ball_pos.yDir > 0) {
      blue_pacman_start = get_blue_pacman_start();
      if (blue_pacman_start == 1) {
        clear_blue_pacman_start();
        return true;
      }
    }
  } else if (pacman->packman_color == GREEN) {
    ball_pos = get_ball_position_direction();
    if (ball_pos.row < matrix_width / 2 && ball_pos.yDir < 0) {
      green_pacman_start = get_green_pacman_start();
      if (green_pacman_start == 1) {
        clear_green_pacman_start();
        return true;
      }
    }
  }
  return false;
}

bool pac_movement_level2(pacman_s *pacman) {
  // // bool blue_pacman_start, green_pacman_start;
  // ball_s ball_pos;
  // if (pacman->packman_color == BLUE) {
  //   ball_pos = get_ball_position_direction();
  //   if (ball_pos.row > matrix_width / 2 && ball_pos.yDir > 0) {
  //     if (ball_pos.xDir == 1 &&pacman.direction = RIGH) {
  //       clear_blue_pacman_start();
  //       return true;
  //       // blue_pacman_setup(&pacman);
  //     }
  //   }
  // } else if (pacman->packman_color == GREEN) {
  //   ball_pos = get_ball_position_direction();
  //   if (ball_pos.row < matrix_width / 2 && ball_pos.yDir < 0) {
  //     green_pacman_start = get_green_pacman_start();
  //     if (green_pacman_start == 1) {
  //       clear_green_pacman_start();
  //       return true;
  //       // green_pacman_setup(&pacman);
  //     }
  //   }
  // }
  // return false;
}