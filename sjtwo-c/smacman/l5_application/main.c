#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <time.h>

#include "acceleration.h"
#include "board_io.h"
#include "common_macros.h"
#include "gpio.h"
#include "sj2_cli.h"

#include "game_graphics.h"

static void smacman__startup(void);

static void blue_pacman_task(void *params);
static void green_pacman_task(void *params);
static void blue_paddle_task(void *params);
static void green_paddle_task(void *params);
static void ball_task(void *params);
static void display_task(void *params);
static void ball_task(void *params);

static gpio_s led0, led1, led2, led3;
static gpio_s switch0, switch1, switch2, switch3;

int main(void) {

  // switch0 = gpio__construct_with_function(GPIO__PORT_0, 29, GPIO__FUNCITON_0_IO_PIN);
  // switch1 = gpio__construct_with_function(GPIO__PORT_0, 30, GPIO__FUNCITON_0_IO_PIN);
  // switch2 = gpio__construct_with_function(GPIO__PORT_1, 15, GPIO__FUNCITON_0_IO_PIN);
  // switch3 = gpio__construct_with_function(GPIO__PORT_1, 19, GPIO__FUNCITON_0_IO_PIN);
  // gpio__set_as_input(switch0);
  // gpio__set_as_input(switch1);
  // gpio__set_as_input(switch2);
  // gpio__set_as_input(switch3);

  // LPC_IOCON->P1_15 |= (1 << 3);
  // LPC_IOCON->P1_19 |= (1 << 3);
  acceleration__axis_data_s sensor_avg_value;
  sensor_avg_value = acceleration__get_data();
  srand(sensor_avg_value.x + sensor_avg_value.z);
  xTaskCreate(blue_pacman_task, "blue_pacman", (2048U / sizeof(void *)), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(green_pacman_task, "green_pacman", (2048U / sizeof(void *)), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(blue_paddle_task, "paddle_blue", (2048U / sizeof(void *)), NULL, PRIORITY_MEDIUM, NULL);
  xTaskCreate(green_paddle_task, "paddle_green", (2048U / sizeof(void *)), NULL, PRIORITY_MEDIUM, NULL);
  xTaskCreate(ball_task, "ball", (2048U / sizeof(void *)), NULL, PRIORITY_HIGH, NULL);
  xTaskCreate(display_task, "Display_Task", (1024U / sizeof(void *)), NULL, PRIORITY_HIGH, NULL);

  // It is advised to either run the uart_task, or the SJ2 command-line (CLI), but not both
  // Change '#if 0' to '#if 1' and vice versa to try it out
#if 0
  // printf() takes more stack space, size this tasks' stack higher
  xTaskCreate(uart_task, "uart", (512U * 8) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
#else
  sj2_cli__init();
#endif

  puts("Starting RTOS");
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler runs out of memory and fails

  return 0;
}

// Position and the velocity of the ball

static void display_task(void *params) {
  led_matrix__displayGridBorders(PINK);
  while (true) {
    led_matrix__update_display();
    vTaskDelay(5);
  }
}

// Columns - along X-axis, Rows - along Y-axis
static void ball_task(void *params) {
  ball_s ball;
  static uint8_t count = 1;
  ball_setup(&ball);
  uint8_t score_blue = 0;
  uint8_t score_green = 0;
  uint8_t cummulative_score = 1;
  while (1) {
    led_matrix_clear_frame_buffer_inside_grid(ALL_INSIDE_GRID);
    count = (count < 4) ? count + 1 : 1;

    // Update x movement // Wall collision
    if (count % (5 - ball.vx) == 0) {
      ball.row = (ball.xDir > 0) ? ball.row + 1 : ball.row - 1; // left: right
      // If wall collision then reverse the x direction
      if (ball.row <= 3 || ball.row >= (matrix_width - 4)) {
        ball.xDir = -ball.xDir;
      }
    }

    if (count % (5 - ball.vy) == 0) {
      ball.col = (ball.yDir > 0) ? ball.col + 1 : ball.col - 1; // up:down

      // Paddle Collision
      bool left_blue = gpio__get(switch0);
      bool right_blue = gpio__get(switch1);
      bool left_green = gpio__get(switch2);
      bool right_green = gpio__get(switch3);

      if (ball.col == 6) {
        if ((led_matrix__get_pixel(ball.row, 3) != 0) || (led_matrix__get_pixel(ball.row - 1, 3) != 0) ||
            (led_matrix__get_pixel(ball.row + 1, 3) != 0)) { // covers 3 rows
          // printf("down : %d, row: %d \n", led_matrix__get_pixel(ball.row, 3), ball.row);
          if ((right_blue && ball.xDir < 0) || (left_blue && ball.xDir > 0)) {
            increase_ball_x(&ball);
          }
          if ((right_blue && ball.xDir > 0) || (left_blue && ball.xDir < 0)) {
            decrease_ball_x(&ball);
          }
          ball.yDir = -ball.yDir;
          cummulative_score++;
          printf("Score to grab: %d \n", cummulative_score);
        }
      }
      // oppponent scores
      if (ball.col == 3) {
        score_green += cummulative_score;
        cummulative_score = 1;
        // led_matrix__cleanBall(ball.row - 1, ball.col - 1);
        printf("GREEN player scored\n");
        printf("BLUE player score: %d   GREEN player score: %d\n", score_blue, score_green);
        ball_setup(&ball);
      }
      if (ball.col == matrix_width - 7) {
        if ((led_matrix__get_pixel(ball.row, matrix_width - 4) != 0) ||
            (led_matrix__get_pixel(ball.row - 1, matrix_width - 4) != 0) ||
            (led_matrix__get_pixel(ball.row + 1, matrix_width - 4) != 0)) {
          // printf("up : %d,  row: %d \n", led_matrix__get_pixel(ball.row, matrix_width - 4), ball.row);
          if ((right_green && ball.xDir < 0) || (left_green && ball.xDir > 0)) {
            increase_ball_x(&ball);
          }
          if ((right_green && ball.xDir > 0) || (left_green && ball.xDir < 0)) {
            decrease_ball_x(&ball);
          }
          ball.yDir = -ball.yDir;
          cummulative_score++;
          printf("Score to grab: %d \n", cummulative_score);
        }
      }

      if (ball.col == matrix_width - 4) {
        // led_matrix__cleanBall(ball.row - 1, ball.col - 1);
        score_blue += cummulative_score;
        cummulative_score = 1;
        printf("BLUE player scored\n");
        printf("BLUE player score: %d   GREEN player score: %d\n", score_blue, score_green);
        ball_setup(&ball);
      }
    }
    detect_collision(ball);
    vTaskDelay(25);
  }
}

// Blue paddle - at the bottom. Directions: LEFT_DOWN, RIGHT_DOWN
// Length of the paddle is 16
static void blue_paddle_task(void *params) {
  int row_start = matrix_width / 4;
  int col = 2;
  for (int i = 0; i < 16; i++) {
    led_matrix__set_pixel(row_start + i, col, BLUE);
    led_matrix__set_pixel(row_start + i, col + 1, BLUE);
  }

  bool left = 0, right = 0;
  led_matrix__direction_e direction;
  while (true) {
    left = gpio__get(switch0);
    right = gpio__get(switch1);
    if (left) {
      direction = LEFT_DOWN;
      switch (row_start) {
      case 2 ... matrix_width - 19:
        led_matrix__drawPaddle_blue(row_start, col, direction);
        row_start++;
        break;
      case matrix_width - 18 ... matrix_width: // Check this condition
        led_matrix__drawPaddle_blue(row_start, col, direction);
        break;
      }
    } else if (right) {
      direction = RIGHT_DOWN;
      switch (row_start + 15) {
      case 18 ... matrix_width - 3:
        led_matrix__drawPaddle_blue(row_start + 15, col, direction);
        row_start--;
        break;
      case 0 ... 17:
        led_matrix__drawPaddle_blue(row_start + 15, col, direction);
        break;
      }
    } else {
      led_matrix__drawPaddle_blue(row_start, col, LEFT_DOWN);
    }

    vTaskDelay(30);
  }
}

// Green paddle - at the top. Directions: LEFT_UP, RIGHT_UP
// Length of the paddle is 16
static void green_paddle_task(void *params) {
  int row_start = matrix_width / 4;
  int col = matrix_height - 3;
  for (int i = 0; i < 16; i++) {
    led_matrix__set_pixel(matrix_width / 4 + i, col, GREEN);
    led_matrix__set_pixel(matrix_width / 4 + i, col - 1, GREEN);
  }
  bool left = 0, right = 0;
  led_matrix__direction_e direction;
  while (true) {
    left = gpio__get(switch2);
    right = gpio__get(switch3);
    if (left) {
      direction = LEFT_UP;
      switch (row_start) {
      case 2 ... matrix_width - 19:
        led_matrix__drawPaddle_green(row_start, col, direction);
        row_start++;
        break;
      case matrix_width - 18 ... matrix_width:
        led_matrix__drawPaddle_green(row_start, col, direction);
        break;
      }
    } else if (right) {
      direction = RIGHT_UP;
      switch (row_start + 15) {
      case 18 ... matrix_width - 3:
        led_matrix__drawPaddle_green(row_start + 15, col, direction);
        row_start--;
        break;
      case 0 ... 17:
        led_matrix__drawPaddle_green(row_start + 15, col, direction);
        break;
      }
    } else {
      led_matrix__drawPaddle_green(row_start, col, LEFT_UP);
    }

    vTaskDelay(50);
  }
}

static void blue_pacman_task(void *params) {
  int row_upordown_right = 5, row_upordown_left = matrix_width - 6, row_left_upordown = 2,
      row_right_upordown = matrix_width - 3;
  int col_up_leftorright = (matrix_height / 2), col_down_leftorright = matrix_height - 5,
      col_leftorrigt_up = matrix_height - 8, col_leftorrigt_down = (matrix_height / 2) + 3;

  int var;
  led_matrix__color_e packman_color = BLUE;
  led_matrix__direction_e direction = LEFT_UP;

  while (true) {
    led_matrix_clear_frame_buffer_inside_grid(UPPER_INSIDE_GRID); // Player 1

    if (direction == LEFT_UP) {
      switch (row_left_upordown) {
      case 2 ... matrix_width - 8:
        game_graphics_packman(row_left_upordown, col_leftorrigt_up, direction, packman_color, PLAYER_1);
        // led_matrix__update_display();
        row_left_upordown++;
        break;
      case matrix_width - 7 ... matrix_width: // Check this condition
        var = rand();
        if (var % 2) {
          direction = DOWN_LEFT;
        } else {
          direction = RIGHT_UP;
        }
        row_left_upordown = 2;
        break;
      }
    }
    if (direction == RIGHT_UP) {
      switch (row_right_upordown) {
      case 7 ... matrix_width - 3:
        game_graphics_packman(row_right_upordown, col_leftorrigt_up, direction, packman_color, PLAYER_1);
        // led_matrix__update_display();
        row_right_upordown--;
        break;
      case 0 ... 6: // Check this condition
        var = rand();
        if (var % 2) {
          direction = LEFT_UP;
        } else {
          direction = DOWN_RIGHT;
        }
        row_right_upordown = matrix_width - 3;
        break;
      }
    }
    if (direction == DOWN_LEFT) {
      switch (col_down_leftorright) {
      case (matrix_height / 2) + 5 ... matrix_height - 5:
        game_graphics_packman(row_upordown_left, col_down_leftorright, direction, packman_color, PLAYER_1);
        // led_matrix__update_display();
        col_down_leftorright--;
        break;
      case 0 ...(matrix_height / 2) + 4: // Check this condition
        var = rand();
        if (var % 2) {
          direction = RIGHT_DOWN;
        } else {
          direction = UP_LEFT;
        }
        col_down_leftorright = matrix_height - 5;
        break;
      }
    }
    if (direction == UP_LEFT) {
      switch (col_up_leftorright) {
      case (matrix_height / 2)... matrix_width - 10:
        game_graphics_packman(row_upordown_left, col_up_leftorright, direction, packman_color, PLAYER_1);
        // led_matrix__update_display();
        col_up_leftorright++;
        break;
      case matrix_height - 9 ... matrix_height:
        var = rand();
        if (var % 2) {
          direction = RIGHT_UP;
        } else {
          direction = DOWN_LEFT;
        }
        col_up_leftorright = (matrix_height / 2);
        break;
      }
    }

    if (direction == RIGHT_DOWN) {
      switch (row_right_upordown) {
      case 7 ... matrix_height - 3:
        game_graphics_packman(row_right_upordown, col_leftorrigt_down, direction, packman_color, PLAYER_1);
        // led_matrix__update_display();
        row_right_upordown--;
        break;
      case 0 ... 6:
        var = rand();
        if (var % 2) {
          direction = UP_RIGHT;
        } else {
          direction = LEFT_DOWN;
        }
        row_right_upordown = matrix_height - 3;
        break;
      }
    }
    if (direction == LEFT_DOWN) {
      switch (row_left_upordown) {
      case 2 ... matrix_width - 8:
        game_graphics_packman(row_left_upordown, col_leftorrigt_down, direction, packman_color, PLAYER_1);
        // led_matrix__update_display();
        row_left_upordown++;
        break;
      case matrix_width - 7 ... matrix_width:
        var = rand();
        if (var % 2) {
          direction = UP_LEFT;
        } else {
          direction = RIGHT_DOWN;
        }
        row_left_upordown = 2;
        break;
      }
    }

    if (direction == UP_RIGHT) {
      switch (col_up_leftorright) {
      case (matrix_height / 2)... matrix_height - 10:
        game_graphics_packman(row_upordown_right, col_up_leftorright, direction, packman_color, PLAYER_1);
        // led_matrix__update_display();
        col_up_leftorright++;
        break;
      case matrix_height - 9 ... matrix_height:
        var = rand();
        if (var % 2) {
          direction = LEFT_UP;
        } else {
          direction = DOWN_RIGHT;
        }
        col_up_leftorright = (matrix_height / 2) + 4;
        break;
      }
    }
    if (direction == DOWN_RIGHT) {
      switch (col_down_leftorright) {
      case (matrix_height / 2) + 5 ... matrix_height - 5:
        game_graphics_packman(row_upordown_right, col_down_leftorright, direction, packman_color, PLAYER_1);
        // led_matrix__update_display();
        col_down_leftorright--;
        break;
      case 0 ...(matrix_height / 2) + 4:
        var = rand();
        if (var % 2) {
          direction = LEFT_DOWN;
        } else {
          direction = UP_RIGHT;
        }
        col_down_leftorright = matrix_height - 5;
        break;
      }
    }

    vTaskDelay(200);
  }
}

static void green_pacman_task(void *params) {
  int row_upordown_right = 5, row_upordown_left = matrix_width - 6, row_left_upordown = 2,
      row_right_upordown = matrix_width - 3;
  int col_up_leftorright = 4, col_down_leftorright = (matrix_height / 2) - 1,
      col_leftorrigt_up = (matrix_height / 2) - 4, col_leftorrigt_down = 7;
  int var;
  led_matrix__color_e packman_color = GREEN;
  led_matrix__direction_e direction = LEFT_UP;

  while (true) {
    led_matrix_clear_frame_buffer_inside_grid(LOWER_INSIDE_GRID); // Player 2
    if (direction == LEFT_UP) {
      switch (row_left_upordown) {
      case 2 ... matrix_width - 8:
        game_graphics_packman(row_left_upordown, col_leftorrigt_up, direction, packman_color, PLAYER_2);
        // led_matrix__update_display();
        row_left_upordown++;
        break;
      case matrix_width - 7 ... matrix_width: // Check this condition
        var = rand();
        if (var % 2) {
          direction = DOWN_LEFT;
        } else {
          direction = RIGHT_UP;
        }
        row_left_upordown = 2;
        break;
      }
    }
    if (direction == RIGHT_UP) {
      switch (row_right_upordown) {
      case 7 ... matrix_width - 3:
        game_graphics_packman(row_right_upordown, col_leftorrigt_up, direction, packman_color, PLAYER_2);
        // led_matrix__update_display();
        row_right_upordown--;
        break;
      case 0 ... 6: // Check this condition
        var = rand();
        if (var % 2) {
          direction = LEFT_UP;
        } else {
          direction = DOWN_RIGHT;
        }
        row_right_upordown = matrix_width - 3;
        break;
      }
    }
    if (direction == DOWN_LEFT) {
      switch (col_down_leftorright) {
      case 9 ...(matrix_height / 2) - 1:
        game_graphics_packman(row_upordown_left, col_down_leftorright, direction, packman_color, PLAYER_2);
        // led_matrix__update_display();
        col_down_leftorright--;
        break;
      case 0 ... 8:
        var = rand();
        if (var % 2) {
          direction = RIGHT_DOWN;
        } else {
          direction = UP_LEFT;
        }
        col_down_leftorright = (matrix_height / 2) - 1;
        break;
      }
    }
    if (direction == UP_LEFT) {
      switch (col_up_leftorright) {
      case 4 ...(matrix_height / 2) - 6:
        game_graphics_packman(row_upordown_left, col_up_leftorright, direction, packman_color, PLAYER_2);
        // led_matrix__update_display();
        col_up_leftorright++;
        break;
      case (matrix_height / 2) - 5 ... matrix_height:
        var = rand();
        if (var % 2) {
          direction = RIGHT_UP;
        } else {
          direction = DOWN_LEFT;
        }
        col_up_leftorright = 4;
        break;
      }
    }

    if (direction == RIGHT_DOWN) {
      switch (row_right_upordown) {
      case 7 ... matrix_height - 3:
        game_graphics_packman(row_right_upordown, col_leftorrigt_down, direction, packman_color, PLAYER_2);
        // led_matrix__update_display();
        row_right_upordown--;
        break;
      case 0 ... 6:
        var = rand();
        if (var % 2) {
          direction = UP_RIGHT;
        } else {
          direction = LEFT_DOWN;
        }
        row_right_upordown = matrix_height - 3;
        break;
      }
    }
    if (direction == LEFT_DOWN) {
      switch (row_left_upordown) {
      case 2 ... matrix_width - 8:
        game_graphics_packman(row_left_upordown, col_leftorrigt_down, direction, packman_color, PLAYER_2);
        // led_matrix__update_display();
        row_left_upordown++;
        break;
      case matrix_width - 7 ... matrix_width:
        var = rand();
        if (var % 2) {
          direction = UP_LEFT;
        } else {
          direction = RIGHT_DOWN;
        }
        row_left_upordown = 2;
        break;
      }
    }

    if (direction == UP_RIGHT) {
      switch (col_up_leftorright) {
      case 4 ...(matrix_height / 2) - 6:
        game_graphics_packman(row_upordown_right, col_up_leftorright, direction, packman_color, PLAYER_2);
        // led_matrix__update_display();
        col_up_leftorright++;
        break;
      case (matrix_height / 2) - 5 ... matrix_height:
        var = rand();
        if (var % 2) {
          direction = LEFT_UP;
        } else {
          direction = DOWN_RIGHT;
        }
        col_up_leftorright = 4;
        break;
      }
    }
    if (direction == DOWN_RIGHT) {
      switch (col_down_leftorright) {
      case 9 ...(matrix_height / 2) - 1:
        game_graphics_packman(row_upordown_right, col_down_leftorright, direction, packman_color,
                              PLAYER_2); // led_matrix__update_display();
        col_down_leftorright--;
        break;
      case 0 ... 8:
        var = rand();
        if (var % 2) {
          direction = LEFT_DOWN;
        } else {
          direction = UP_RIGHT;
        }
        col_down_leftorright = (matrix_height / 2) - 1;
        break;
      }
    }

    vTaskDelay(200);
  }
}

static void smacman__startup(void) {
  led0 = gpio__construct_as_output(GPIO__PORT_1, 18);
  led1 = gpio__construct_as_output(GPIO__PORT_1, 24);
  led2 = gpio__construct_as_output(GPIO__PORT_1, 26);
  led3 = gpio__construct_as_output(GPIO__PORT_2, 3);
  switch0 = gpio__construct_as_input(GPIO__PORT_0, 29);
  switch1 = gpio__construct_as_input(GPIO__PORT_0, 30);
  switch2 = gpio__construct_as_input(GPIO__PORT_0, 8);
  switch3 = gpio__construct_as_input(GPIO__PORT_0, 9);

  gpio__set(led0);
  gpio__set(led1);
  gpio__set(led2);
  gpio__set(led3);

  led_matrix__init();
}