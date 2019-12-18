#include "score_graphics.h"

// void display_score(int score_blue, int score_green) {
void score_task(void *task_param) {

  uint8_t score_blue = 0, score_green = 0;
  uint8_t blue_digit1 = 0, blue_digit2 = 0, green_digit1 = 0, green_digit2 = 0;

  while (1) {
    score_blue = get_blue_player_score();
    score_green = get_green_player_score();
    green_digit1 = score_green % 10;
    green_digit2 = (score_green - green_digit1) / 10;
    blue_digit1 = score_blue % 10;
    blue_digit2 = (score_blue - blue_digit1) / 10;
    led_matrix_clear_frame_buffer_inside_grid(0x3FFFFFFFFFFFFFFC);
    display_digit(green_digit1, 48, 19, GREEN);
    display_digit(green_digit2, 34, 19, GREEN);
    display_digit(blue_digit1, 18, 19, BLUE);
    display_digit(blue_digit2, 4, 19, BLUE);
    vTaskDelay(25);
  }
  // led_matrix__fill_frame_buffer_inside_grid();
}
