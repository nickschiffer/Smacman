#include "score_graphics.h"

void display_score(int score_blue, int score_green) {

  uint8_t blue_digit1 = 0, blue_digit2 = 0, green_digit1 = 0, green_digit2 = 0;
  green_digit1 = score_green % 10;
  green_digit2 = (score_green - green_digit1) / 10;

  blue_digit1 = score_blue % 10;
  blue_digit2 = (score_blue - blue_digit1) / 10;
  led_matrix__fill_frame_buffer_inside_grid();
  display_digit(green_digit1, 45, 47, GREEN);
  display_digit(green_digit2, 59, 47, GREEN);
  display_digit(blue_digit1, 15, 47, BLUE);
  display_digit(blue_digit2, 29, 47, BLUE);
  vTaskDelay(400);
  led_matrix__fill_frame_buffer_inside_grid();
}
