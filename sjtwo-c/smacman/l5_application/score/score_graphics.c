#include "score_graphics.h"

void display_digit(int digit, int row, int column, led_matrix__color_e player_color) {
  // led_matrix__fill_frame_buffer_inside_grid();
  // int row, column;
  bool segment[8] = {0};
  switch (digit) {
    // clang  off
  case 0:
    segment[0] = 1;
    segment[1] = 1;
    segment[2] = 1;
    segment[3] = 1;
    segment[4] = 1;
    segment[5] = 1;
    segment[6] = 0;
    break;
  case 1:
    segment[0] = 0;
    segment[1] = 1;
    segment[2] = 1;
    segment[3] = 0;
    segment[4] = 0;
    segment[5] = 0;
    segment[6] = 0;
    break;
  case 2:
    segment[0] = 1;
    segment[1] = 1;
    segment[2] = 0;
    segment[3] = 1;
    segment[4] = 1;
    segment[5] = 0;
    segment[6] = 1;
    break;
  case 3:
    segment[0] = 1;
    segment[1] = 1;
    segment[2] = 1;
    segment[3] = 1;
    segment[4] = 0;
    segment[5] = 0;
    segment[6] = 1;
    break;
  case 4:
    segment[0] = 0;
    segment[1] = 1;
    segment[2] = 1;
    segment[3] = 0;
    segment[4] = 0;
    segment[5] = 1;
    segment[6] = 1;
    break;
  case 5:
    segment[0] = 1;
    segment[1] = 0;
    segment[2] = 1;
    segment[3] = 1;
    segment[4] = 0;
    segment[5] = 1;
    segment[6] = 1;
    break;
  case 6:
    segment[0] = 1;
    segment[1] = 0;
    segment[2] = 1;
    segment[3] = 1;
    segment[4] = 1;
    segment[5] = 1;
    segment[6] = 1;
    break;
  case 7:
    segment[0] = 1;
    segment[1] = 1;
    segment[2] = 1;
    segment[3] = 0;
    segment[4] = 0;
    segment[5] = 0;
    segment[6] = 0;
    break;
  case 8:
    segment[0] = 1;
    segment[1] = 1;
    segment[2] = 1;
    segment[3] = 1;
    segment[4] = 1;
    segment[5] = 1;
    segment[6] = 1;
    break;
  case 9:
    segment[0] = 1;
    segment[1] = 1;
    segment[2] = 1;
    segment[3] = 1;
    segment[4] = 0;
    segment[5] = 1;
    segment[6] = 1;
    break;

    // clanf  on
  }

  if (segment[0] == 1) {
    for (int i = 0; i < 12; i++) {
      led_matrix__set_pixel(row + i, column, player_color);
      led_matrix__set_pixel(row + i, column + 1, player_color);
    }
  }
  if (segment[1] == 1) {
    for (int i = 0; i < 12; i++) {
      led_matrix__set_pixel(row + 10, column + 2 + i, player_color);
      led_matrix__set_pixel(row + 11, column + 2 + i, player_color);
    }
  }
  if (segment[2] == 1) {
    for (int i = 0; i < 12; i++) {
      led_matrix__set_pixel(row + 10, column + 16 + i, player_color);
      led_matrix__set_pixel(row + 11, column + 16 + i, player_color);
    }
  }
  if (segment[3] == 1) {
    for (int i = 0; i < 12; i++) {
      led_matrix__set_pixel(row + i, column + 28, player_color);
      led_matrix__set_pixel(row + i, column + 29, player_color);
    }
  }
  if (segment[4] == 1) {
    for (int i = 0; i < 12; i++) {
      led_matrix__set_pixel(row, column + 16 + i, player_color);
      led_matrix__set_pixel(row + 1, column + 16 + i, player_color);
    }
  }
  if (segment[5] == 1) {
    for (int i = 0; i < 12; i++) {
      led_matrix__set_pixel(row, column + 2 + i, player_color);
      led_matrix__set_pixel(row + 1, column + 2 + i, player_color);
    }
  }
  if (segment[6] == 1) {
    for (int i = 0; i < 12; i++) {
      led_matrix__set_pixel(row + i, column + 14, player_color);
      led_matrix__set_pixel(row + i, column + 15, player_color);
    }
  }
}
