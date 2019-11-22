#include "led_graphics.h"

void led_matrix__displayGridBorders(led_matrix__color_e color) {
  data_size columns_borders = 0xC000000000000003;
  data_size row_borders = 0xFFFFFFFFFFFFFFFF;
  led_matrix__set_row_data(0, color, row_borders);
  led_matrix__set_row_data(1, color, row_borders);
  led_matrix__set_row_data((matrix_width - 1), color, row_borders);
  led_matrix__set_row_data((matrix_width - 2), color, row_borders);
  for (uint32_t i = 2; i < matrix_width - 2; i++) {
    led_matrix__set_row_data(i, color, columns_borders);
  }
}

void led_matrix__drawPaddle_blue(int row, int column, led_matrix__direction_e direction) {
  data_size data = ~((data_size)0x000000000000000C);
  for (int i = 2; i < (matrix_width - 2); i++) {
    frame_buffer[i][RED_PLANE] &= data;
    frame_buffer[i][GREEN_PLANE] &= data;
    frame_buffer[i][BLUE_PLANE] &= data;
  }
  led_matrix__color_e paddle1_color = BLUE;
  if (direction == LEFT_DOWN) {
    for (int i = 0; i < 16; i++) {
      led_matrix__set_pixel(row + i, column, paddle1_color);
      led_matrix__set_pixel(row + i, column + 1, paddle1_color);
    }
  }
  if (direction == RIGHT_DOWN) {
    for (int i = 0; i < 16; i++) {
      led_matrix__set_pixel(row - i, column, paddle1_color);
      led_matrix__set_pixel(row - i, column + 1, paddle1_color);
    }
  }
}
void led_matrix__drawPaddle_green(int row, int column, led_matrix__direction_e direction) {
  data_size data = ~((data_size)0x3000000000000000);
  for (int i = 2; i < (matrix_width - 2); i++) {
    frame_buffer[i][RED_PLANE] &= data;
    frame_buffer[i][GREEN_PLANE] &= data;
    frame_buffer[i][BLUE_PLANE] &= data;
  }
  led_matrix__color_e paddle2_color = GREEN;
  if (direction == LEFT_UP) {
    for (int i = 0; i < 16; i++) {
      led_matrix__set_pixel(row + i, column, paddle2_color);
      led_matrix__set_pixel(row + i, column - 1, paddle2_color);
    }
  }
  if (direction == RIGHT_UP) {
    for (int i = 0; i < 16; i++) {
      led_matrix__set_pixel(row - i, column, paddle2_color);
      led_matrix__set_pixel(row - i, column - 1, paddle2_color);
    }
  }
}

/**************Setting  Lines for Pacman- Begins*****************/
void led_matrix__set_line1_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right,
                               led_matrix__color_e packman_color) {
  if (var_left || var_right) {
    led_matrix__set_pixel(row, column, packman_color);
    led_matrix__set_pixel(row, column - 1, packman_color);
    led_matrix__set_pixel(row, column + 1, packman_color);
  } else if (var_up || var_down) {
    led_matrix__set_pixel(row, column, packman_color);
    led_matrix__set_pixel(row - 1, column, packman_color);
    led_matrix__set_pixel(row + 1, column, packman_color);
  }
}
void led_matrix__set_line2_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right,
                               led_matrix__color_e packman_color) {

  if (var_left || var_right) {
    led_matrix__set_pixel(var_left ? row + 1 : row - 1, column + 2, packman_color);
    led_matrix__set_pixel(var_left ? row + 1 : row - 1, column + 1, packman_color);
    led_matrix__set_pixel(var_left ? row + 1 : row - 1, column + 0, packman_color);
    led_matrix__set_pixel(var_left ? row + 1 : row - 1, column - 1, packman_color);
    led_matrix__set_pixel(var_left ? row + 1 : row - 1, column - 2, packman_color);
  } else if (var_up || var_down) {
    led_matrix__set_pixel(row + 2, var_down ? column - 1 : column + 1, packman_color);
    led_matrix__set_pixel(row + 1, var_down ? column - 1 : column + 1, packman_color);
    led_matrix__set_pixel(row + 0, var_down ? column - 1 : column + 1, packman_color);
    led_matrix__set_pixel(row - 1, var_down ? column - 1 : column + 1, packman_color);
    led_matrix__set_pixel(row - 2, var_down ? column - 1 : column + 1, packman_color);
  }
}
void led_matrix__set_line3_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right,
                               led_matrix__color_e packman_color) {

  if (var_left || var_right) {
    led_matrix__set_pixel(var_left ? row + 2 : row - 2, column + 3, packman_color);
    led_matrix__set_pixel(var_left ? row + 2 : row - 2, column + 2, packman_color);
    led_matrix__set_pixel(var_left ? row + 2 : row - 2, column + 1, packman_color);
    led_matrix__set_pixel(var_left ? row + 2 : row - 2, column + 0, packman_color);
    led_matrix__set_pixel(var_left ? row + 2 : row - 2, column - 1, packman_color);
    led_matrix__set_pixel(var_left ? row + 2 : row - 2, column - 2, packman_color);
    led_matrix__set_pixel(var_left ? row + 2 : row - 2, column - 3, packman_color);
  } else if (var_up || var_down) {
    led_matrix__set_pixel(row + 3, var_down ? column - 2 : column + 2, packman_color);
    led_matrix__set_pixel(row + 2, var_down ? column - 2 : column + 2, packman_color);
    led_matrix__set_pixel(row + 1, var_down ? column - 2 : column + 2, packman_color);
    led_matrix__set_pixel(row + 0, var_down ? column - 2 : column + 2, packman_color);
    led_matrix__set_pixel(row - 1, var_down ? column - 2 : column + 2, packman_color);
    led_matrix__set_pixel(row - 2, var_down ? column - 2 : column + 2, packman_color);
    led_matrix__set_pixel(row - 3, var_down ? column - 2 : column + 2, packman_color);
  }
}
void led_matrix__set_line4_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right,
                               led_matrix__color_e packman_color) {

  if (var_left || var_right) {
    led_matrix__set_pixel(var_left ? row + 3 : row - 3, column - 3, packman_color);
    led_matrix__set_pixel(var_left ? row + 3 : row - 3, column - 2, packman_color);
    led_matrix__set_pixel(var_left ? row + 3 : row - 3, column - 1, packman_color);
    led_matrix__set_pixel(var_left ? row + 3 : row - 3, column + 1, packman_color);
    led_matrix__set_pixel(var_left ? row + 3 : row - 3, column + 2, packman_color);
    led_matrix__set_pixel(var_left ? row + 3 : row - 3, column + 3, packman_color);
  } else if (var_up || var_down) {
    led_matrix__set_pixel(row + 3, var_down ? column - 3 : column + 3, packman_color);
    led_matrix__set_pixel(row + 2, var_down ? column - 3 : column + 3, packman_color);
    led_matrix__set_pixel(row + 1, var_down ? column - 3 : column + 3, packman_color);
    led_matrix__set_pixel(row - 1, var_down ? column - 3 : column + 3, packman_color);
    led_matrix__set_pixel(row - 2, var_down ? column - 3 : column + 3, packman_color);
    led_matrix__set_pixel(row - 3, var_down ? column - 3 : column + 3, packman_color);
  }
}
void led_matrix__set_line5_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right,
                               led_matrix__color_e packman_color) {

  if (var_left || var_right) {
    led_matrix__set_pixel(var_left ? row + 4 : row - 4, column + 3, packman_color);
    led_matrix__set_pixel(var_left ? row + 4 : row - 4, column + 2, packman_color);
    led_matrix__set_pixel(var_left ? row + 4 : row - 4, column - 2, packman_color);
    led_matrix__set_pixel(var_left ? row + 4 : row - 4, column - 3, packman_color);
  } else if (var_up || var_down) {
    led_matrix__set_pixel(row + 3, var_down ? column - 4 : column + 4, packman_color);
    led_matrix__set_pixel(row + 2, var_down ? column - 4 : column + 4, packman_color);
    led_matrix__set_pixel(row - 2, var_down ? column - 4 : column + 4, packman_color);
    led_matrix__set_pixel(row - 3, var_down ? column - 4 : column + 4, packman_color);
  }
}
void led_matrix__set_line6_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right,
                               led_matrix__color_e packman_color) {

  if (var_left || var_right) {
    led_matrix__set_pixel(var_left ? row + 5 : row - 5, column + 3, packman_color);
    led_matrix__set_pixel(var_left ? row + 5 : row - 5, column - 3, packman_color);
  } else if (var_up || var_down) {
    led_matrix__set_pixel(row + 3, var_down ? column - 5 : column + 5, packman_color);
    led_matrix__set_pixel(row - 3, var_down ? column - 5 : column + 5, packman_color);
  }
}
/**************Setting  Lines for Ball- Begins*****************/
void led_matrix__set_line1_ball(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right) {
  led_matrix__color_e ball_color = RED;
  if (var_left || var_right) {
    led_matrix__set_pixel(row, column, ball_color);
    led_matrix__set_pixel(row, column - 1, ball_color);
    led_matrix__set_pixel(row, column + 1, ball_color);
  } else if (var_up || var_down) {
    led_matrix__set_pixel(row, column, ball_color);
    led_matrix__set_pixel(row - 1, column, ball_color);
    led_matrix__set_pixel(row + 1, column, ball_color);
  }
}
void led_matrix__set_line2_ball(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right) {
  led_matrix__color_e ball_color = RED;
  if (var_left || var_right) {
    led_matrix__set_pixel(var_left ? row + 1 : row - 1, column + 2, ball_color);
    led_matrix__set_pixel(var_left ? row + 1 : row - 1, column + 1, ball_color);
    led_matrix__set_pixel(var_left ? row + 1 : row - 1, column + 0, ball_color);
    led_matrix__set_pixel(var_left ? row + 1 : row - 1, column - 1, ball_color);
    led_matrix__set_pixel(var_left ? row + 1 : row - 1, column - 2, ball_color);
  } else if (var_up || var_down) {
    led_matrix__set_pixel(row + 2, var_down ? column - 1 : column + 1, ball_color);
    led_matrix__set_pixel(row + 1, var_down ? column - 1 : column + 1, ball_color);
    led_matrix__set_pixel(row + 0, var_down ? column - 1 : column + 1, ball_color);
    led_matrix__set_pixel(row - 1, var_down ? column - 1 : column + 1, ball_color);
    led_matrix__set_pixel(row - 2, var_down ? column - 1 : column + 1, ball_color);
  }
}
void led_matrix__set_line3_ball(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right) {
  led_matrix__color_e ball_color = RED;
  if (var_left || var_right) {
    led_matrix__set_pixel(var_left ? row + 2 : row - 2, column + 2, ball_color);
    led_matrix__set_pixel(var_left ? row + 2 : row - 2, column + 1, ball_color);
    led_matrix__set_pixel(var_left ? row + 2 : row - 2, column + 0, ball_color);
    led_matrix__set_pixel(var_left ? row + 2 : row - 2, column - 1, ball_color);
    led_matrix__set_pixel(var_left ? row + 2 : row - 2, column - 2, ball_color);
  } else if (var_up || var_down) {
    led_matrix__set_pixel(row + 2, var_down ? column - 2 : column + 2, ball_color);
    led_matrix__set_pixel(row + 1, var_down ? column - 2 : column + 2, ball_color);
    led_matrix__set_pixel(row + 0, var_down ? column - 2 : column + 2, ball_color);
    led_matrix__set_pixel(row - 1, var_down ? column - 2 : column + 2, ball_color);
    led_matrix__set_pixel(row - 2, var_down ? column - 2 : column + 2, ball_color);
  }
}
void led_matrix__set_line4_ball(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right) {
  led_matrix__color_e ball_color = RED;
  if (var_left || var_right) {
    led_matrix__set_pixel(var_left ? row + 3 : row - 3, column - 2, ball_color);
    led_matrix__set_pixel(var_left ? row + 3 : row - 3, column - 1, ball_color);
    led_matrix__set_pixel(var_left ? row + 3 : row - 3, column + 1, ball_color);
    led_matrix__set_pixel(var_left ? row + 3 : row - 3, column + 2, ball_color);
    led_matrix__set_pixel(var_left ? row + 3 : row - 3, column + 0, ball_color);
  } else if (var_up || var_down) {
    led_matrix__set_pixel(row + 0, var_down ? column - 3 : column + 3, ball_color);
    led_matrix__set_pixel(row + 2, var_down ? column - 3 : column + 3, ball_color);
    led_matrix__set_pixel(row + 1, var_down ? column - 3 : column + 3, ball_color);
    led_matrix__set_pixel(row - 1, var_down ? column - 3 : column + 3, ball_color);
    led_matrix__set_pixel(row - 2, var_down ? column - 3 : column + 3, ball_color);
  }
}
void led_matrix__set_line5_ball(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right) {
  led_matrix__color_e ball_color = RED;
  if (var_left || var_right) {
    led_matrix__set_pixel(var_left ? row + 4 : row - 4, column + 0, ball_color);
    led_matrix__set_pixel(var_left ? row + 4 : row - 4, column + 1, ball_color);
    led_matrix__set_pixel(var_left ? row + 4 : row - 4, column - 1, ball_color);
  } else if (var_up || var_down) {
    led_matrix__set_pixel(row + 1, var_down ? column - 4 : column + 4, ball_color);
    led_matrix__set_pixel(row + 0, var_down ? column - 4 : column + 4, ball_color);
    led_matrix__set_pixel(row - 1, var_down ? column - 4 : column + 4, ball_color);
  }
}

/**************Setting Lines for Ball - Ends*****************/

void led_matrix__drawBall(int row, int column) {
  led_matrix__color_e ball_color = RED;
  led_matrix__set_pixel(row, column, ball_color);
  led_matrix__set_pixel(row, column - 1, ball_color);
  led_matrix__set_pixel(row, column + 1, ball_color);
  led_matrix__set_pixel(row, column - 2, ball_color);
  led_matrix__set_pixel(row, column + 2, ball_color);

  led_matrix__set_pixel(row - 1, column, ball_color);
  led_matrix__set_pixel(row - 1, column - 1, ball_color);
  led_matrix__set_pixel(row - 1, column + 1, ball_color);
  led_matrix__set_pixel(row - 1, column - 2, ball_color);
  led_matrix__set_pixel(row - 1, column + 2, ball_color);

  led_matrix__set_pixel(row + 1, column, ball_color);
  led_matrix__set_pixel(row + 1, column - 1, ball_color);
  led_matrix__set_pixel(row + 1, column + 1, ball_color);
  led_matrix__set_pixel(row + 1, column - 2, ball_color);
  led_matrix__set_pixel(row + 1, column + 2, ball_color);

  led_matrix__set_pixel(row - 2, column, ball_color);
  led_matrix__set_pixel(row - 2, column - 1, ball_color);
  led_matrix__set_pixel(row - 2, column + 1, ball_color);

  led_matrix__set_pixel(row + 2, column, ball_color);
  led_matrix__set_pixel(row + 2, column - 1, ball_color);
  led_matrix__set_pixel(row + 2, column + 1, ball_color);
}

/**************Setting Lines for Pacman - Ends*****************/

void led_matrix__drawPackMan(int row, int column, led_matrix__direction_e direction,
                             led_matrix__color_e packman_color) {

  bool var_up = 0, var_down = 0, var_left = 0, var_right = 0;

  if (direction == LEFT_UP || direction == LEFT_DOWN) {
    var_left = 1;
  }
  if (direction == UP_RIGHT || direction == UP_LEFT) {
    var_up = 1;
  }
  if (direction == RIGHT_DOWN || direction == RIGHT_UP) {
    var_right = 1;
  }
  if (direction == DOWN_LEFT || direction == DOWN_RIGHT) {
    var_down = 1;
  }
  led_matrix__set_line1_pac(row, column, var_up, var_down, var_left, var_right, packman_color);
  led_matrix__set_line2_pac(row, column, var_up, var_down, var_left, var_right, packman_color);
  led_matrix__set_line3_pac(row, column, var_up, var_down, var_left, var_right, packman_color);
  led_matrix__set_line4_pac(row, column, var_up, var_down, var_left, var_right, packman_color);
  led_matrix__set_line5_pac(row, column, var_up, var_down, var_left, var_right, packman_color);
  led_matrix__set_line6_pac(row, column, var_up, var_down, var_left, var_right, packman_color);
}
