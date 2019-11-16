#include "led_graphics.h"

// void displayGridBorders(Color color) {
//   uint64_t columns_borders = 0xC000000000000003;
//   uint64_t row_borders = 0xFFFFFFFFFFFFFFFF;
//   for (int i = 0; i < 64; i++) {
//     if ((i == 0) || (i == 1) || (i == 62 || (i == 63))) {
//       led_matrix__set_row_data(i, color, row_borders);
//     } else {
//       led_matrix__set_row_data(i, color, columns_borders);
//     }
//   }
// }

void displayGridBorders(led_matrix__color_e color) {
  uint64_t columns_borders = 0x0000000000000000;
  uint64_t row_borders = 0xFFFFFFFFFFFFFFFF;
  for (int i = 0; i < 64; i++) {
    if (i >= 0 && i < 32) {
      led_matrix__set_row_data(i, BLUE, row_borders);
    } else {
      led_matrix__set_row_data(i, RED, row_borders);
    }
  }
}

void drawPackMan(int row, int column) {
  led_matrix__color_e packman_color = LIME;
  led_matrix__set_pixel(row, column, packman_color);
  led_matrix__set_pixel(row, column - 1, packman_color);
  led_matrix__set_pixel(row, column + 1, packman_color);

  led_matrix__set_pixel(row + 1, column + 2, packman_color);
  led_matrix__set_pixel(row + 1, column + 1, packman_color);
  led_matrix__set_pixel(row + 1, column + 0, packman_color);
  led_matrix__set_pixel(row + 1, column - 1, packman_color);
  led_matrix__set_pixel(row + 1, column - 2, packman_color);

  led_matrix__set_pixel(row + 2, column + 3, packman_color);
  led_matrix__set_pixel(row + 2, column + 2, packman_color);
  led_matrix__set_pixel(row + 2, column + 1, packman_color);
  led_matrix__set_pixel(row + 2, column + 0, packman_color);
  led_matrix__set_pixel(row + 2, column - 1, packman_color);
  led_matrix__set_pixel(row + 2, column - 2, packman_color);
  led_matrix__set_pixel(row + 2, column - 3, packman_color);

  led_matrix__set_pixel(row + 3, column - 3, packman_color);
  led_matrix__set_pixel(row + 3, column - 2, packman_color);
  led_matrix__set_pixel(row + 3, column - 1, packman_color);
  led_matrix__set_pixel(row + 3, column + 1, packman_color);
  led_matrix__set_pixel(row + 3, column + 2, packman_color);
  led_matrix__set_pixel(row + 3, column + 3, packman_color);

  led_matrix__set_pixel(row + 4, column + 3, packman_color);
  led_matrix__set_pixel(row + 4, column + 2, packman_color);
  led_matrix__set_pixel(row + 4, column - 2, packman_color);
  led_matrix__set_pixel(row + 4, column - 3, packman_color);

  led_matrix__set_pixel(row + 5, column + 3, packman_color);
  led_matrix__set_pixel(row + 5, column - 3, packman_color);
  led_matrix__set_pixel(row + 6, column, packman_color);
}