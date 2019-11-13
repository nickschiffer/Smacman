#include "led_matrix_32.h"

void led_matrix_init() {
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

  A = gpio__construct_as_output(GPIO__PORT_2, 7);  // Grey
  B = gpio__construct_as_output(GPIO__PORT_2, 8);  // Marron
  C = gpio__construct_as_output(GPIO__PORT_2, 9);  // Orange
  D = gpio__construct_as_output(GPIO__PORT_0, 16); // voilet
  E = gpio__construct_as_output(GPIO__PORT_0, 15); // yellow

  oe = gpio__construct_as_output(GPIO__PORT_1, 20);    // gray
  latch = gpio__construct_as_output(GPIO__PORT_1, 23); // white
  clk = gpio__construct_as_output(GPIO__PORT_1, 28);   // brown

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
  disable_display();
  disable_latch();
  gpio__reset(clk);
}

void enable_display() { gpio__reset(oe); }

void disable_display() { gpio__set(oe); }

void enable_latch() { gpio__set(latch); }

void disable_latch() { gpio__reset(latch); }

void selectRow(int row) {
  (row & 0x01) ? gpio__set(A) : gpio__reset(A);
  (row & 0x02) ? gpio__set(B) : gpio__reset(B);
  (row & 0x04) ? gpio__set(C) : gpio__reset(C);
  (row & 0x08) ? gpio__set(D) : gpio__reset(D);
  (row & 0x10) ? gpio__set(E) : gpio__reset(E);
}

void clearPixel(int row, int column) {
  data_size pixel = ~((data_size)1 << column);
  farmeBuffer[row][RedPlane] &= pixel;
  farmeBuffer[row][GreenPlane] &= pixel;
  farmeBuffer[row][BluePlane] &= pixel;
}

void setPixel(int row, int col, Color color) {
  data_size pixel = ((data_size)1 << col);
  if (color & 0x01) {
    farmeBuffer[row][BluePlane] |= pixel;
  }
  if (color & 0x02) {
    farmeBuffer[row][GreenPlane] |= pixel;
  }
  if (color & 0x04) {
    farmeBuffer[row][RedPlane] |= pixel;
  }
}

void setRowData(uint32_t row, Color color, data_size data) {
  if (color & 0x01) {
    farmeBuffer[row][BluePlane] = data;
  }
  if (color & 0x02) {
    farmeBuffer[row][GreenPlane] = data;
  }
  if (color & 0x04) {
    farmeBuffer[row][RedPlane] = data;
  }
}

void setRowDataRaw(int row, ColorPlane plane, uint64_t data) { farmeBuffer[row][plane] = data; }

void clearFrameBuffer() { memset(farmeBuffer, 0, sizeof(farmeBuffer)); }

void fillFrameBuffer(data_size data, Color color) {
  for (int i = 0; i < row_bound; i++) {
    setRowData(i, color, data);
  }
}

void fillFrameBuffer_Inside_Grid_Black() {
  data_size data = ~((data_size)0x3FFFFFFC);
  for (int i = 2; i < (matrix_width - 2); i++) {
    farmeBuffer[i][RedPlane] &= data;
    farmeBuffer[i][GreenPlane] &= data;
    farmeBuffer[i][BluePlane] &= data;
  }
}

void updateDisplay() {
  for (int i = 0; i < 32; i++) {
    disable_display();
    disable_latch();
    selectRow(i); // will select i and i + 32 rows at same time

    for (int j = 63; j >= 0; j--) { // shift data with MSB getting shifted-in first
      ((farmeBuffer[i][RedPlane] >> j) & 1) ? gpio__set(r1) : gpio__reset(r1);
      ((farmeBuffer[i][GreenPlane] >> j) & 1) ? gpio__set(g1) : gpio__reset(g1);
      ((farmeBuffer[i][BluePlane] >> j) & 1) ? gpio__set(b1) : gpio__reset(b1);

      ((farmeBuffer[i + 32][RedPlane] >> j) & 1) ? gpio__set(r2) : gpio__reset(r2);
      ((farmeBuffer[i + 32][GreenPlane] >> j) & 1) ? gpio__set(g2) : gpio__reset(g2);
      ((farmeBuffer[i + 32][BluePlane] >> j) & 1) ? gpio__set(b2) : gpio__reset(b2);
      gpio__set(clk);
      gpio__reset(clk); // shift in all 3 color bits at once for top half/bottom half registers
      enable_latch();
      disable_latch();
    }
    // at this point, all 3 shift registers should be filled with corresponding row data in frameBuffer
    enable_latch(); // push shift register contents down to output registers
    enable_display();
    delay__us(150);
  }
  disable_display();
}
