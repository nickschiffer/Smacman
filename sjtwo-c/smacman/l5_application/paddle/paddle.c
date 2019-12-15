#include "paddle.h"

gpio_s switch0, switch1, switch2, switch3;

void paddle_task(void *task_param) {
  paddle_s pad_setup = *(paddle_s *)task_param;
  int row_start = pad_setup.row_start; // matrix_width / 4;
  int col = pad_setup.col;
  for (int i = 0; i < 16; i++) {
    led_matrix__set_pixel(row_start + i, col, BLUE);
    led_matrix__set_pixel(row_start + i, col + 1, BLUE);
  }
  bool left = 0, right = 0;
  led_matrix__direction_e direction;
  while (true) {
    if (pad_setup.paddle_color == BLUE) {
#if SMACMAN_CONTROLLER_CONNECTED
      uint16_t dir = controller_comm__get_player_1_accel();
      if (dir == 0) {
        left = 0;
        right = 0;
      } else if (dir < 2000) {
        left = 1;
      } else {
        right = 1;
      }
#else
      left = gpio__get(switch0);
      right = gpio__get(switch1);
#endif
      if (left) {
        paddle_direction_e blue_pacman_direction = PADDLE_DIRECTION_LEFT;
        xQueueSend(blue_paddle_direction_queue, &blue_pacman_direction, 0);
      } else if (right) {
        paddle_direction_e blue_pacman_direction = PADDLE_DIRECTION_RIGHT;
        xQueueSend(blue_paddle_direction_queue, &blue_pacman_direction, 0);
      }

    } else {
#if SMACMAN_CONTROLLER_CONNECTED
      uint16_t dir = controller_comm__get_player_2_accel();
      if (dir == 0) {
        left = 0;
        right = 0;
      } else if (dir < 2000) {
        left = 1;
      } else {
        right = 1;
      }
#else
      left = gpio__get(switch2);
      right = gpio__get(switch3);
#endif
      if (left) {
        paddle_direction_e green_pacman_direction = PADDLE_DIRECTION_LEFT;
        xQueueSend(green_paddle_direction_queue, &green_pacman_direction, 0);
      } else if (right) {
        paddle_direction_e green_pacman_direction = PADDLE_DIRECTION_RIGHT;
        xQueueSend(green_paddle_direction_queue, &green_pacman_direction, 0);
      }
    }
    if (left) {
      direction = LEFT_DOWN;
      switch (row_start) {
      case 2 ... matrix_width - 19:
        led_matrix__drawPaddle(row_start, col, direction, pad_setup.paddle_color);
        row_start++;
        break;
      case matrix_width - 18 ... matrix_width: // Check this condition
        led_matrix__drawPaddle(row_start, col, direction, pad_setup.paddle_color);
        break;
      }
    } else if (right) {
      direction = RIGHT_DOWN;
      switch (row_start + 15) {
      case 18 ... matrix_width - 3:
        led_matrix__drawPaddle(row_start + 15, col, direction, pad_setup.paddle_color);
        row_start--;
        break;
      case 0 ... 17:
        led_matrix__drawPaddle(row_start + 15, col, direction, pad_setup.paddle_color);
        break;
      }
    } else {
      led_matrix__drawPaddle(row_start, col, LEFT_DOWN, pad_setup.paddle_color);
    }
    vTaskDelay(30);
  }
}
