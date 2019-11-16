
#include "led_matrix.h"

void led_matrix__init() {
  r1 = gpio__construct_with_function(GPIO__PORT_2, 0, GPIO__FUNCITON_0_IO_PIN);
  g1 = gpio__construct_with_function(GPIO__PORT_2, 1, GPIO__FUNCITON_0_IO_PIN);
  b1 = gpio__construct_with_function(GPIO__PORT_2, 2, GPIO__FUNCITON_0_IO_PIN);
  r2 = gpio__construct_with_function(GPIO__PORT_2, 4, GPIO__FUNCITON_0_IO_PIN);
  g2 = gpio__construct_with_function(GPIO__PORT_2, 5, GPIO__FUNCITON_0_IO_PIN);
  b2 = gpio__construct_with_function(GPIO__PORT_2, 6, GPIO__FUNCITON_0_IO_PIN);

  A = gpio__construct_with_function(GPIO__PORT_2, 7, GPIO__FUNCITON_0_IO_PIN);
  B = gpio__construct_with_function(GPIO__PORT_2, 8, GPIO__FUNCITON_0_IO_PIN);
  C = gpio__construct_with_function(GPIO__PORT_2, 9, GPIO__FUNCITON_0_IO_PIN);
  D = gpio__construct_with_function(GPIO__PORT_0, 16, GPIO__FUNCITON_0_IO_PIN);
  E = gpio__construct_with_function(GPIO__PORT_0, 15, GPIO__FUNCITON_0_IO_PIN);

  oe = gpio__construct_with_function(GPIO__PORT_1, 20, GPIO__FUNCITON_0_IO_PIN);
  latch = gpio__construct_with_function(GPIO__PORT_1, 23, GPIO__FUNCITON_0_IO_PIN);
  clk = gpio__construct_with_function(GPIO__PORT_1, 28, GPIO__FUNCITON_0_IO_PIN);

  r1 = gpio__construct_as_output(GPIO__PORT_2, 0);
  g1 = gpio__construct_as_output(GPIO__PORT_2, 1);
  b1 = gpio__construct_as_output(GPIO__PORT_2, 2);
  r2 = gpio__construct_as_output(GPIO__PORT_2, 4);
  g2 = gpio__construct_as_output(GPIO__PORT_2, 5);
  b2 = gpio__construct_as_output(GPIO__PORT_2, 6);

  A = gpio__construct_as_output(GPIO__PORT_2, 7);
  B = gpio__construct_as_output(GPIO__PORT_2, 8);
  C = gpio__construct_as_output(GPIO__PORT_2, 9);
  D = gpio__construct_as_output(GPIO__PORT_0, 16);
  E = gpio__construct_as_output(GPIO__PORT_0, 15);

  oe = gpio__construct_as_output(GPIO__PORT_1, 20);
  latch = gpio__construct_as_output(GPIO__PORT_1, 23);
  clk = gpio__construct_as_output(GPIO__PORT_1, 28);

  gpio__reset(A);
  gpio__reset(B);
  gpio__reset(C);
  gpio__reset(D);
  gpio__reset(E);
  gpio__reset(r1);
  gpio__reset(g1);
  gpio__reset(b1);
  gpio__reset(r2);
  gpio__reset(g2);
  gpio__reset(b2);
  led_matrix__disable_display();
  led_matrix__disable_latch();
  gpio__reset(clk);
}

void led_matrix__enable_display() { gpio__reset(oe); }

void led_matrix__disable_display() { gpio__set(oe); }

void led_matrix__enable_latch() { gpio__set(latch); }

void led_matrix__disable_latch() { gpio__reset(latch); }

void led_matrix__select_row(int row) {
  (row & 0x01) ? gpio__set(A) : gpio__reset(A);
  (row & 0x02) ? gpio__set(B) : gpio__reset(B);
  (row & 0x04) ? gpio__set(C) : gpio__reset(C);
  (row & 0x08) ? gpio__set(D) : gpio__reset(D);
  (row & 0x10) ? gpio__set(E) : gpio__reset(E);
}

void led_matrix__clear_pixel(int row, int column) {
  uint64_t pixel = ~((uint64_t)1 << column);
  frame_buffer[row][RED_PLANE] &= pixel;
  frame_buffer[row][GREEN_PLANE] &= pixel;
  frame_buffer[row][BLUE_PLANE] &= pixel;
}

void led_matrix__set_pixel(int row, int col, led_matrix__color_e color) {
  uint64_t pixel = ((uint64_t)1 << col);
  if (color & 0x01) {
    frame_buffer[row][BLUE_PLANE] |= pixel;
  }
  if (color & 0x02) {
    frame_buffer[row][GREEN_PLANE] |= pixel;
  }
  if (color & 0x04) {
    frame_buffer[row][RED_PLANE] |= pixel;
  }
}

void led_matrix__set_row_data(int row, led_matrix__color_e color, uint64_t data) {
  if (color & 0x01) {
    frame_buffer[row][BLUE_PLANE] = data;
  }
  if (color & 0x02) {
    frame_buffer[row][GREEN_PLANE] = data;
  }
  if (color & 0x04) {
    frame_buffer[row][RED_PLANE] = data;
  }
}

void led_matrix__set_row_data_raw(int row, led_matrix__color_plane_e plane, uint64_t data) {
  frame_buffer[row][plane] = data;
}

void led_matrix__clear_frame_buffer() { memset(frame_buffer, 0, sizeof(frame_buffer)); }

void led_matrix__fill_frame_buffer(uint64_t data, led_matrix__color_e color) {
  for (int i = 0; i < 64; i++) {
    led_matrix__set_row_data(i, color, data);
  }
}

void led_matrix__update_display() {
  for (int i = 0; i < 32; i++) {
    led_matrix__disable_display();
    led_matrix__disable_latch();
    led_matrix__select_row(i); // will select i and i + 32 rows at same time

    for (int j = 63; j >= 0; j--) { // shift data with MSB getting shifted-in first
      ((frame_buffer[i][RED_PLANE] >> j) & 1) ? gpio__set(r1) : gpio__reset(r1);
      ((frame_buffer[i][GREEN_PLANE] >> j) & 1) ? gpio__set(g1) : gpio__reset(g1);
      ((frame_buffer[i][BLUE_PLANE] >> j) & 1) ? gpio__set(b1) : gpio__reset(b1);

      ((frame_buffer[i + 32][RED_PLANE] >> j) & 1) ? gpio__set(r2) : gpio__reset(r2);
      ((frame_buffer[i + 32][GREEN_PLANE] >> j) & 1) ? gpio__set(g2) : gpio__reset(g2);
      ((frame_buffer[i + 32][BLUE_PLANE] >> j) & 1) ? gpio__set(b2) : gpio__reset(b2);
      gpio__set(clk);
      gpio__reset(clk); // shift in all 3 color bits at once for top half/bottom half registers
      led_matrix__enable_latch();
      led_matrix__disable_latch();
    }
    // at this point, all 3 shift registers should be filled with corresponding row data in frame_buffer
    led_matrix__enable_latch(); // push shift register contents down to output registers
    led_matrix__enable_display();
    delay__us(250);
  }
  led_matrix__disable_display();
}