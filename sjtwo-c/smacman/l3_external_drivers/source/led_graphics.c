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

void led_matrix__drawPaddle(int row, int column, led_matrix__direction_e direction, led_matrix__color_e paddle_color) {
  data_size data;
  if (paddle_color == GREEN) {
    data = ~((data_size)0x3000000000000000);
  } else { // if (paddle_color ==BLUE)
    data = ~((data_size)0x000000000000000C);
  }
  for (int i = 2; i < (matrix_width - 2); i++) {
    frame_buffer[i][RED_PLANE] &= data;
    frame_buffer[i][GREEN_PLANE] &= data;
    frame_buffer[i][BLUE_PLANE] &= data;
  }
  if (direction == LEFT_DOWN) {
    for (int i = 0; i < 16; i++) {
      led_matrix__set_pixel(row + i, column, paddle_color);
      led_matrix__set_pixel(row + i, column + 1, paddle_color);
    }
  } else if (direction == RIGHT_DOWN) {
    for (int i = 0; i < 16; i++) {
      led_matrix__set_pixel(row - i, column, paddle_color);
      led_matrix__set_pixel(row - i, column + 1, paddle_color);
    }
  } else {
  }
}

/**************Setting  Lines for Pacman- Begins*****************/
void led_matrix__set_line1_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right,
                               led_matrix__color_e packman_color) {
  if (var_left || var_right) {
    led_matrix__modify_pixel(row, column, packman_color);
    led_matrix__modify_pixel(row, column - 1, packman_color);
    led_matrix__modify_pixel(row, column + 1, packman_color);
  } else if (var_up || var_down) {
    led_matrix__modify_pixel(row, column, packman_color);
    led_matrix__modify_pixel(row - 1, column, packman_color);
    led_matrix__modify_pixel(row + 1, column, packman_color);
  }
}
void led_matrix__set_line2_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right,
                               led_matrix__color_e packman_color) {

  if (var_left || var_right) {
    led_matrix__modify_pixel(var_left ? row + 1 : row - 1, column + 2, packman_color);
    led_matrix__modify_pixel(var_left ? row + 1 : row - 1, column + 1, packman_color);
    led_matrix__modify_pixel(var_left ? row + 1 : row - 1, column + 0, packman_color);
    led_matrix__modify_pixel(var_left ? row + 1 : row - 1, column - 1, packman_color);
    led_matrix__modify_pixel(var_left ? row + 1 : row - 1, column - 2, packman_color);
  } else if (var_up || var_down) {
    led_matrix__modify_pixel(row + 2, var_down ? column - 1 : column + 1, packman_color);
    led_matrix__modify_pixel(row + 1, var_down ? column - 1 : column + 1, packman_color);
    led_matrix__modify_pixel(row + 0, var_down ? column - 1 : column + 1, packman_color);
    led_matrix__modify_pixel(row - 1, var_down ? column - 1 : column + 1, packman_color);
    led_matrix__modify_pixel(row - 2, var_down ? column - 1 : column + 1, packman_color);
  }
}
void led_matrix__set_line3_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right,
                               led_matrix__color_e packman_color) {

  if (var_left || var_right) {
    led_matrix__modify_pixel(var_left ? row + 2 : row - 2, column + 3, packman_color);
    led_matrix__modify_pixel(var_left ? row + 2 : row - 2, column + 2, packman_color);
    led_matrix__modify_pixel(var_left ? row + 2 : row - 2, column + 1, packman_color);
    led_matrix__modify_pixel(var_left ? row + 2 : row - 2, column + 0, packman_color);
    led_matrix__modify_pixel(var_left ? row + 2 : row - 2, column - 1, packman_color);
    led_matrix__modify_pixel(var_left ? row + 2 : row - 2, column - 2, packman_color);
    led_matrix__modify_pixel(var_left ? row + 2 : row - 2, column - 3, packman_color);
  } else if (var_up || var_down) {
    led_matrix__modify_pixel(row + 3, var_down ? column - 2 : column + 2, packman_color);
    led_matrix__modify_pixel(row + 2, var_down ? column - 2 : column + 2, packman_color);
    led_matrix__modify_pixel(row + 1, var_down ? column - 2 : column + 2, packman_color);
    led_matrix__modify_pixel(row + 0, var_down ? column - 2 : column + 2, packman_color);
    led_matrix__modify_pixel(row - 1, var_down ? column - 2 : column + 2, packman_color);
    led_matrix__modify_pixel(row - 2, var_down ? column - 2 : column + 2, packman_color);
    led_matrix__modify_pixel(row - 3, var_down ? column - 2 : column + 2, packman_color);
  }
}
void led_matrix__set_line4_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right,
                               led_matrix__color_e packman_color) {

  if (var_left || var_right) {
    led_matrix__modify_pixel(var_left ? row + 3 : row - 3, column - 3, packman_color);
    led_matrix__modify_pixel(var_left ? row + 3 : row - 3, column - 2, packman_color);
    led_matrix__modify_pixel(var_left ? row + 3 : row - 3, column - 1, packman_color);
    led_matrix__modify_pixel(var_left ? row + 3 : row - 3, column + 1, packman_color);
    led_matrix__modify_pixel(var_left ? row + 3 : row - 3, column + 2, packman_color);
    led_matrix__modify_pixel(var_left ? row + 3 : row - 3, column + 3, packman_color);
  } else if (var_up || var_down) {
    led_matrix__modify_pixel(row + 3, var_down ? column - 3 : column + 3, packman_color);
    led_matrix__modify_pixel(row + 2, var_down ? column - 3 : column + 3, packman_color);
    led_matrix__modify_pixel(row + 1, var_down ? column - 3 : column + 3, packman_color);
    led_matrix__modify_pixel(row - 1, var_down ? column - 3 : column + 3, packman_color);
    led_matrix__modify_pixel(row - 2, var_down ? column - 3 : column + 3, packman_color);
    led_matrix__modify_pixel(row - 3, var_down ? column - 3 : column + 3, packman_color);
  }
}
void led_matrix__set_line5_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right,
                               led_matrix__color_e packman_color) {

  if (var_left || var_right) {
    led_matrix__modify_pixel(var_left ? row + 4 : row - 4, column + 3, packman_color);
    led_matrix__modify_pixel(var_left ? row + 4 : row - 4, column + 2, packman_color);
    led_matrix__modify_pixel(var_left ? row + 4 : row - 4, column - 2, packman_color);
    led_matrix__modify_pixel(var_left ? row + 4 : row - 4, column - 3, packman_color);
  } else if (var_up || var_down) {
    led_matrix__modify_pixel(row + 3, var_down ? column - 4 : column + 4, packman_color);
    led_matrix__modify_pixel(row + 2, var_down ? column - 4 : column + 4, packman_color);
    led_matrix__modify_pixel(row - 2, var_down ? column - 4 : column + 4, packman_color);
    led_matrix__modify_pixel(row - 3, var_down ? column - 4 : column + 4, packman_color);
  }
}
void led_matrix__set_line6_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right,
                               led_matrix__color_e packman_color) {

  if (var_left || var_right) {
    led_matrix__modify_pixel(var_left ? row + 5 : row - 5, column + 3, packman_color);
    led_matrix__modify_pixel(var_left ? row + 5 : row - 5, column - 3, packman_color);
  } else if (var_up || var_down) {
    led_matrix__modify_pixel(row + 3, var_down ? column - 5 : column + 5, packman_color);
    led_matrix__modify_pixel(row - 3, var_down ? column - 5 : column + 5, packman_color);
  }
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

void led_matrix__drawBall(int row, int column, led_matrix__color_e ball_color) {
  // led_matrix__color_e ball_color = RED;
  led_matrix__modify_pixel(row, column, ball_color);
  led_matrix__modify_pixel(row, column - 1, ball_color);
  led_matrix__modify_pixel(row, column + 1, ball_color);
  led_matrix__modify_pixel(row, column - 2, ball_color);
  led_matrix__modify_pixel(row, column + 2, ball_color);

  led_matrix__modify_pixel(row - 1, column, ball_color);
  led_matrix__modify_pixel(row - 1, column - 1, ball_color);
  led_matrix__modify_pixel(row - 1, column + 1, ball_color);
  led_matrix__modify_pixel(row - 1, column - 2, ball_color);
  led_matrix__modify_pixel(row - 1, column + 2, ball_color);

  led_matrix__modify_pixel(row + 1, column, ball_color);
  led_matrix__modify_pixel(row + 1, column - 1, ball_color);
  led_matrix__modify_pixel(row + 1, column + 1, ball_color);
  led_matrix__modify_pixel(row + 1, column - 2, ball_color);
  led_matrix__modify_pixel(row + 1, column + 2, ball_color);

  led_matrix__modify_pixel(row - 2, column, ball_color);
  led_matrix__modify_pixel(row - 2, column - 1, ball_color);
  led_matrix__modify_pixel(row - 2, column + 1, ball_color);

  led_matrix__modify_pixel(row + 2, column, ball_color);
  led_matrix__modify_pixel(row + 2, column - 1, ball_color);
  led_matrix__modify_pixel(row + 2, column + 1, ball_color);
}

// intro tasks starts
void led_matrix__display_intro_GridBorder(void) {
  led_matrix__fill_frame_buffer_inside_grid();
  led_matrix__color_e border_color = WHITE;
  data_size data = ((data_size)0x00000C0000300000);
  for (uint32_t i = 0; i < matrix_width; i++) {
    led_matrix__set_row_data(i, border_color, data);
  }
  led_matrix__display_intro_Ball();
}

void led_matrix__display_intro_Ball(void) {
  led_matrix__color_e ball_color = RED;
  int8_t temp1 = 13, temp2 = 0, count = 1;
  int row = (matrix_width / 2), col = (matrix_width / 2) - 7; // 38
  for (int8_t i = 0; i < 7; i++) {
    for (int8_t j = temp1; j > temp2; j--) {
      led_matrix__set_pixel(row + i, col + j, ball_color);
      led_matrix__set_pixel(row - i, col + j, ball_color);
    }
    count++;
    if (count % 2) {
      temp1--;
      temp2++;
    }
  }

  led_matrix__set_pixel(row + 4, col + 12, ball_color);
  led_matrix__set_pixel(row - 4, col + 12, ball_color);
  led_matrix__set_pixel(row + 4, col + 2, ball_color);
  led_matrix__set_pixel(row - 4, col + 2, ball_color);
  led_matrix__set_pixel(row + 2, col + 13, ball_color);
  led_matrix__set_pixel(row - 2, col + 13, ball_color);
  led_matrix__set_pixel(row + 2, col + 1, ball_color);
  led_matrix__set_pixel(row - 2, col + 1, ball_color);
  led_matrix__display_intro_Pacs();
  led_matrix__display_intro_lines();
}

void led_matrix__display_intro_Pacs(void) {
  led_matrix__color_e pac1_color = BLUE, pac2_color = GREEN;
  int8_t temp1 = 0, temp2 = 4, count = 0;
  int row = matrix_width - 10, col = (matrix_width / 2), row2 = 10; // 38
  for (int8_t i = 0; i < 16; i++) {
    for (int8_t j = temp1; j < temp2; j++) {
      led_matrix__set_pixel(row - i, col + j, pac1_color);
      led_matrix__set_pixel(row - i, col - j, pac1_color);
      led_matrix__set_pixel(row2 + i, col + j, pac2_color);
      led_matrix__set_pixel(row2 + i, col - j, pac2_color);
    }
    if (temp2 < 9) {
      temp2++;
    }
    count++;
    if (count > 7) {
      temp1++;
    }
  }
  led_matrix__clear_pixel(row - 10, col - 8);
  led_matrix__clear_pixel(row - 10, col + 8);
  led_matrix__clear_pixel(row - 11, col - 8);
  led_matrix__clear_pixel(row - 11, col + 8);
  led_matrix__clear_pixel(row - 11, col - 7);
  led_matrix__clear_pixel(row - 11, col + 7);
  led_matrix__clear_pixel(row - 12, col - 8);
  led_matrix__clear_pixel(row - 12, col + 8);
  led_matrix__clear_pixel(row - 12, col - 7);
  led_matrix__clear_pixel(row - 12, col + 7);
  led_matrix__clear_pixel(row - 13, col - 7);
  led_matrix__clear_pixel(row - 13, col + 7);
  led_matrix__clear_pixel(row - 13, col - 6);
  led_matrix__clear_pixel(row - 13, col + 6);
  led_matrix__clear_pixel(row - 13, col - 8);
  led_matrix__clear_pixel(row - 13, col + 8);
  led_matrix__clear_pixel(row - 14, col - 8);
  led_matrix__clear_pixel(row - 14, col + 8);
  led_matrix__clear_pixel(row - 14, col - 7);
  led_matrix__clear_pixel(row - 14, col + 7);
  led_matrix__clear_pixel(row - 15, col - 8);
  led_matrix__clear_pixel(row - 15, col + 8);

  // green packman
  led_matrix__clear_pixel(row2 + 10, col - 8);
  led_matrix__clear_pixel(row2 + 10, col + 8);
  led_matrix__clear_pixel(row2 + 11, col - 8);
  led_matrix__clear_pixel(row2 + 11, col + 8);
  led_matrix__clear_pixel(row2 + 11, col - 7);
  led_matrix__clear_pixel(row2 + 11, col + 7);
  led_matrix__clear_pixel(row2 + 12, col - 8);
  led_matrix__clear_pixel(row2 + 12, col + 8);
  led_matrix__clear_pixel(row2 + 12, col - 7);
  led_matrix__clear_pixel(row2 + 12, col + 7);
  led_matrix__clear_pixel(row2 + 13, col - 7);
  led_matrix__clear_pixel(row2 + 13, col + 7);
  led_matrix__clear_pixel(row2 + 13, col - 6);
  led_matrix__clear_pixel(row2 + 13, col + 6);
  led_matrix__clear_pixel(row2 + 13, col - 8);
  led_matrix__clear_pixel(row2 + 13, col + 8);
  led_matrix__clear_pixel(row2 + 14, col - 8);
  led_matrix__clear_pixel(row2 + 14, col + 8);
  led_matrix__clear_pixel(row2 + 14, col - 7);
  led_matrix__clear_pixel(row2 + 14, col + 7);
  led_matrix__clear_pixel(row2 + 15, col - 8);
  led_matrix__clear_pixel(row2 + 15, col + 8);

  // led_matrix__display_intro_Ball();
  // led_matrix__clear_pixel(row - 14, col - 6);
  // led_matrix__clear_pixel(row - 14, col + 6);
  // led_matrix__set_pixel(row + 4, col + 2, ball_color);
  // led_matrix__set_pixel(row - 4, col + 2, ball_color);
  // led_matrix__set_pixel(row + 2, col + 13, ball_color);
  // led_matrix__set_pixel(row - 2, col + 13, ball_color);
  // led_matrix__set_pixel(row + 2, col + 1, ball_color);
  // led_matrix__set_pixel(row - 2, col + 1, ball_color);
}

void led_matrix__display_intro_lines(void) {
  led_matrix__color_e line_color = LIME;

  led_graphics__draw_alphabet('s', 3, 4, line_color);
  led_graphics__draw_alphabet('m', 11, 4, line_color);
  led_graphics__draw_alphabet('a', 21, 4, line_color);
  led_graphics__draw_alphabet('c', 29, 4, line_color);
  led_graphics__draw_alphabet('m', 29 + 6 + 2, 4, line_color);
  led_graphics__draw_alphabet('a', 38 + 6 + 2, 4, line_color);
  led_graphics__draw_alphabet('n', 46 + 7, 4, line_color);

  led_graphics__draw_alphabet('s', 13, 49, CYAN);
  led_graphics__draw_alphabet('t', 21, 49, CYAN);
  led_graphics__draw_alphabet('a', 30, 49, CYAN);
  led_graphics__draw_alphabet('r', 38, 49, CYAN);
  led_graphics__draw_alphabet('t', 46, 49, CYAN);
}

// intro task ends

void led_graphics__draw_alphabet(char alphabet, int row, int col, led_matrix__color_e line_color) {
  switch (alphabet) {
  case 'S':
  case 's':
    led_graphics__draw_line_upto_offset(row, col, 6, line_color);
    led_graphics__draw_line_upto_offset(row, col + 1, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 2, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 3, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 4, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 5, 6, line_color);
    led_graphics__draw_line_upto_offset(row + 5, col + 6, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 5, col + 7, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 5, col + 8, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 5, col + 9, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 10, 6, line_color);
    break;
  default:
    break;
  case 'M':
  case 'm':
    led_graphics__draw_line_upto_offset(row, col, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 1, col + 1, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 2, col + 2, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 1, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 2, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 3, col + 3, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 3, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 4, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 5, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 6, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 7, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 8, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 9, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 10, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 7, col, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 7, col + 1, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 6, col + 1, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 7, col + 2, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 5, col + 2, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 7, col + 3, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 4, col + 3, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 7, col + 4, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 7, col + 5, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 7, col + 6, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 7, col + 7, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 7, col + 8, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 7, col + 9, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 7, col + 10, 1, line_color);
    break;
  case 'A':
  case 'a':
    led_graphics__draw_line_upto_offset(row, col, 6, line_color);
    led_graphics__draw_line_upto_offset(row, col + 1, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 5, col + 1, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 2, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 5, col + 2, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 3, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 5, col + 3, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 4, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 5, col + 4, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 5, 6, line_color);
    led_graphics__draw_line_upto_offset(row, col + 6, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 7, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 8, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 9, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 10, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 5, col + 6, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 5, col + 7, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 5, col + 8, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 5, col + 9, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 5, col + 10, 1, line_color);
    break;
  case 'C':
  case 'c':
    led_graphics__draw_line_upto_offset(row, col + 0, 6, line_color);
    led_graphics__draw_line_upto_offset(row, col + 1, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 2, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 3, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 4, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 5, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 6, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 7, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 8, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 9, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 10, 6, line_color);
    break;
  case 'N':
  case 'n':
    led_graphics__draw_line_upto_offset(row, col + 0, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 1, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 2, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 3, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 4, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 5, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 6, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 7, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 8, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 9, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 10, 1, line_color);

    led_graphics__draw_line_upto_offset(row + 1, col + 1, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 6, col + 9, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 1, col + 2, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 6, col + 8, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 2, col + 3, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 5, col + 7, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 3, col + 4, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 4, col + 6, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 4, col + 5, 1, line_color);

    led_graphics__draw_line_upto_offset(row + 7, col + 0, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 7, col + 1, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 7, col + 2, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 7, col + 3, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 7, col + 4, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 7, col + 5, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 7, col + 6, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 7, col + 7, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 7, col + 8, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 7, col + 9, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 7, col + 10, 1, line_color);
    break;

  case 'R':
  case 'r':
    led_graphics__draw_line_upto_offset(row, col, 6, line_color);
    led_graphics__draw_line_upto_offset(row, col + 1, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 5, col + 1, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 2, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 5, col + 2, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 3, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 5, col + 3, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 4, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 5, col + 4, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 5, 6, line_color);
    led_graphics__draw_line_upto_offset(row, col + 6, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 7, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 8, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 9, 1, line_color);
    led_graphics__draw_line_upto_offset(row, col + 10, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 1, col + 6, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 2, col + 7, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 3, col + 8, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 4, col + 9, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 5, col + 10, 1, line_color);
    break;

  case 'T':
  case 't':
    led_graphics__draw_line_upto_offset(row, col, 7, line_color);
    led_graphics__draw_line_upto_offset(row + 3, col + 1, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 3, col + 2, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 3, col + 3, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 3, col + 4, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 3, col + 5, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 3, col + 6, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 3, col + 7, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 3, col + 8, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 3, col + 9, 1, line_color);
    led_graphics__draw_line_upto_offset(row + 3, col + 10, 1, line_color);
    break;
  }
}

void led_graphics__draw_line_upto_offset(int row, int col, int offset, led_matrix__color_e line_color) {
  for (int i = 0; i < offset; i++)
    led_matrix__set_pixel(row + i, col, line_color);
}
