#include "led_graphics.h"

// void displayGridBorders(Color color) {
//   uint64_t columns_borders = 0xC000000000000003;
//   uint64_t row_borders = 0xFFFFFFFFFFFFFFFF;
//   for (int i = 0; i < 64; i++) {
//     if ((i == 0) || (i == 1) || (i == 62 || (i == 63))) {
//       setRowData(i, color, row_borders);
//     } else {
//       setRowData(i, color, columns_borders);
//     }
//   }
// }

void displayGridBorders(Color color) {
  uint64_t columns_borders = 0x0000000000000000;
  uint64_t row_borders = 0xFFFFFFFFFFFFFFFF;
  for (int i = 0; i < 64; i++) {
    if (i >= 0 && i < 32) {
      setRowData(i, Blue, row_borders);
    } else {
      setRowData(i, Red, row_borders);
    }
  }
}

void drawPackMan(int row, int column) {
  Color packman_color = Lime;
  setPixel(row, column, packman_color);
  setPixel(row, column - 1, packman_color);
  setPixel(row, column + 1, packman_color);

  setPixel(row + 1, column + 2, packman_color);
  setPixel(row + 1, column + 1, packman_color);
  setPixel(row + 1, column + 0, packman_color);
  setPixel(row + 1, column - 1, packman_color);
  setPixel(row + 1, column - 2, packman_color);

  setPixel(row + 2, column + 3, packman_color);
  setPixel(row + 2, column + 2, packman_color);
  setPixel(row + 2, column + 1, packman_color);
  setPixel(row + 2, column + 0, packman_color);
  setPixel(row + 2, column - 1, packman_color);
  setPixel(row + 2, column - 2, packman_color);
  setPixel(row + 2, column - 3, packman_color);

  setPixel(row + 3, column - 3, packman_color);
  setPixel(row + 3, column - 2, packman_color);
  setPixel(row + 3, column - 1, packman_color);
  setPixel(row + 3, column + 1, packman_color);
  setPixel(row + 3, column + 2, packman_color);
  setPixel(row + 3, column + 3, packman_color);

  setPixel(row + 4, column + 3, packman_color);
  setPixel(row + 4, column + 2, packman_color);
  setPixel(row + 4, column - 2, packman_color);
  setPixel(row + 4, column - 3, packman_color);

  setPixel(row + 5, column + 3, packman_color);
  setPixel(row + 5, column - 3, packman_color);
  setPixel(row + 6, column, packman_color);
}