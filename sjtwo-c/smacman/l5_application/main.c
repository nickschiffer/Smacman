#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <time.h>

#include "acceleration.h"
#include "board_io.h"
#include "common_macros.h"
#include "gpio.h"
#include "sj2_cli.h"

#include "game_logic.h"
#include "mp3.h"
#include "semphr.h"

xSemaphoreHandle mutex_for_spi;
extern volatile bool collided;
static bool green_pacman_start = 0;
static bool blue_pacman_start = 0;

static void smacman__startup(void);

static void master_task(void *params);
static void blue_pacman_task(void *params);
static void green_pacman_task(void *params);
static void blue_paddle_task(void *params);
static void green_paddle_task(void *params);
static void ball_task(void *params);
static void display_task(void *params);

static BaseType_t create_task_game(void *function_name, char *task_name, uint32_t stack_size, uint8_t priority,
                                   TaskHandle_t *xTaskHandle, bool is_suspended);

static gpio_s led0, led1, led2, led3;
static gpio_s switch0, switch1, switch2, switch3;

static void file_read(void *params) {
  // open_file("song.mp3", &fp);
  while (1) {
    // if (xSemaphoreTake(mutex_for_spi, 1000)) {
    if (!mp3_read_from_sd()) {
      fprintf(stderr, "Failed Sleeping imdefinetly\n");
      vTaskDelay(1000);
      // break;
    }
    xSemaphoreGive(mutex_for_spi);
    // }
  }
}

static void mp3_play(void *params) {
  while (1) {
    if (xSemaphoreTake(mutex_for_spi, 1000)) {
      mp3_play_song_from_queue();
      // xSemaphoreGive(mutex_for_spi);
    }
  }
}

int main(void) {
  smacman__startup();
  // mutex_for_spi = xSemaphoreCreateMutex();
  mutex_for_spi = xSemaphoreCreateBinary();
  // ssp2__initialize(24 * 1000);

  acceleration__axis_data_s sensor_avg_value;
  sensor_avg_value = acceleration__get_data();
  srand(sensor_avg_value.x + sensor_avg_value.z);

  // mp3_init();
  // file_init();
  // uint8_t v = (sci_read(VS1053_REG_STATUS) >> 4) & 0x0F;
  // printf("Version: %d\n", v);
  // printf("Mode = 0x%x\n", sci_read(VS1053_REG_MODE));
  // printf("Stat = 0x%x\n", sci_read(VS1053_REG_STATUS));
  // printf("ClkF = 0x%x\n", sci_read(VS1053_REG_CLOCKF));
  // printf("Vol. = 0x%x\n", sci_read(VS1053_REG_VOLUME));
  xTaskCreate(master_task, "master_task", (4096U / sizeof(void *)), NULL, PRIORITY_CRITICAL, NULL);
  xTaskCreate(display_task, "Display_Task", (1024U / sizeof(void *)), NULL, PRIORITY_HIGH, NULL);
  // if (v == 4) {
  // xTaskCreate(file_read, "file_read", (8192U / sizeof(void *)), NULL, PRIORITY_LOW, NULL);
  // xTaskCreate(mp3_play, "mp3_play", (8192U / sizeof(void *)), NULL, PRIORITY_HIGH, NULL);
  // }

  // xTaskCreateRestricted()

#if 0
  // SMACMAN__DEBUG_PRINTF() takes more stack space, size this tasks' stack higher
  xTaskCreate(uart_task, "uart", (512U * 8) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
#else
  sj2_cli__init();
#endif

  puts("Starting RTOS");
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler runs out of memory and fails

  return 0;
}

// Position and the velocity of the ball

static BaseType_t create_task_game(void *function_name, char *task_name, uint32_t stack_size, uint8_t priority,
                                   TaskHandle_t *xTaskHandle, bool is_suspended) {
  BaseType_t xReturned;
  xReturned = xTaskCreate(function_name, task_name, (stack_size / sizeof(void *)), NULL, priority, xTaskHandle);
  if (xReturned == pdPASS) {
    SMACMAN__DEBUG_PRINTF("%s Task Created Successfully\n", task_name);
  } else {
    SMACMAN__DEBUG_PRINTF("%s Task Not Created\n", task_name);
    return pdFAIL;
  }
  vTaskSuspend(*xTaskHandle);
  return xReturned;
}

static void master_task(void *params) {
  game_logic_game_state_s game_current_state = INIT_STATE;
  BaseType_t xReturned;
  TaskHandle_t xHandle[5] = {NULL};
  while (1) {
    switch (game_current_state) {
    case INIT_STATE:
      xReturned =
          create_task_game(blue_pacman_task, "blue_pacman", 2048, PRIORITY_LOW, &xHandle[blue_pacman], TASK_SUSPENDED);
      xReturned = create_task_game(green_pacman_task, "green_pacman", 2048, PRIORITY_LOW, &xHandle[green_pacman],
                                   TASK_SUSPENDED);
      xReturned = create_task_game(blue_paddle_task, "paddle_blue", 2048, PRIORITY_MEDIUM, &xHandle[paddle_blue],
                                   TASK_SUSPENDED);
      xReturned = create_task_game(green_paddle_task, "paddle_green", 2048, PRIORITY_MEDIUM, &xHandle[paddle_green],
                                   TASK_SUSPENDED);
      xReturned = create_task_game(ball_task, "ball", 2048, PRIORITY_HIGH, &xHandle[ball], TASK_SUSPENDED);
      game_current_state = IN_PROGRESS_STATE;
      break;
    case IN_PROGRESS_STATE:
      vTaskResume(xHandle[blue_pacman]);
      vTaskResume(xHandle[green_pacman]);
      vTaskResume(xHandle[paddle_blue]);
      vTaskResume(xHandle[paddle_green]);
      vTaskResume(xHandle[ball]);
    default:
      SMACMAN__DEBUG_PRINTF("Game in progress\n");
      break;
    }
    vTaskDelay(1000);
  }
}

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
    // led_matrix_clear_frame_buffer_inside_grid(ALL_INSIDE_GRID);
    led_matrix__drawBall(ball.row, ball.col, OFF);
    count = (count < 4) ? count + 1 : 1;

    // Update x movement // Wall collision
    if (count % (5 - ball.vx) == 0) {
      ball.row = (ball.xDir > 0) ? ball.row + 1 : ball.row - 1; // left: right
      // If wall collision then reverse the x direction
      if (ball.row <= 3 || ball.row >= (matrix_width - 5)) {
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
          // SMACMAN__DEBUG_PRINTF("down : %d, row: %d \n", led_matrix__get_pixel(ball.row, 3), ball.row);
          if ((right_blue && ball.xDir < 0) || (left_blue && ball.xDir > 0)) {
            increase_ball_x(&ball);
          }
          if ((right_blue && ball.xDir > 0) || (left_blue && ball.xDir < 0)) {
            decrease_ball_x(&ball);
          }
          ball.yDir = -ball.yDir;
          cummulative_score++;
          SMACMAN__DEBUG_PRINTF("Score to grab: %d \n", cummulative_score);
        }
      }
      // oppponent scores
      if (ball.col == 3) {
        score_green += cummulative_score;
        cummulative_score = 1;
        // led_matrix__cleanBall(ball.row - 1, ball.col - 1);
        SMACMAN__DEBUG_PRINTF("GREEN player scored\n");
        SMACMAN__DEBUG_PRINTF("BLUE player score: %d   GREEN player score: %d\n", score_blue, score_green);
        ball_setup(&ball);
      }
      if (ball.col == matrix_width - 7) {
        if ((led_matrix__get_pixel(ball.row, matrix_width - 4) != 0) ||
            (led_matrix__get_pixel(ball.row - 1, matrix_width - 4) != 0) ||
            (led_matrix__get_pixel(ball.row + 1, matrix_width - 4) != 0)) {
          // SMACMAN__DEBUG_PRINTF("up : %d,  row: %d \n", led_matrix__get_pixel(ball.row, matrix_width - 4), ball.row);
          if ((right_green && ball.xDir < 0) || (left_green && ball.xDir > 0)) {
            increase_ball_x(&ball);
          }
          if ((right_green && ball.xDir > 0) || (left_green && ball.xDir < 0)) {
            decrease_ball_x(&ball);
          }
          ball.yDir = -ball.yDir;
          cummulative_score++;
          SMACMAN__DEBUG_PRINTF("Score to grab: %d \n", cummulative_score);
        }
      }

      if (ball.col == matrix_width - 4) {
        // led_matrix__cleanBall(ball.row - 1, ball.col - 1);
        score_blue += cummulative_score;
        cummulative_score = 1;
        SMACMAN__DEBUG_PRINTF("BLUE player scored\n");
        SMACMAN__DEBUG_PRINTF("BLUE player score: %d   GREEN player score: %d\n", score_blue, score_green);
        ball_setup(&ball);
      }
    }
    detect_collision(ball);
    if (collided == 1) {
      collided = 0;
      blue_pacman_start = 1;
      green_pacman_start = 1;
      ball_setup(&ball);
    }
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
  pacman_s blue_pacman;
  blue_pacman.row_upordown_right = 5, blue_pacman.row_upordown_left = matrix_width - 6,
  blue_pacman.row_left_upordown = 2, blue_pacman.row_right_upordown = matrix_width - 3;
  blue_pacman.col_up_leftorright = (matrix_height / 2), blue_pacman.col_down_leftorright = matrix_height - 5,
  blue_pacman.col_leftorrigt_up = matrix_height - 8, blue_pacman.col_leftorrigt_down = (matrix_height / 2) + 3;

  int var;
  blue_pacman.packman_color = BLUE;
  blue_pacman.direction = LEFT_UP;

  while (true) {
    led_matrix__fill_frame_buffer_inside_grid_upper_half(); // Player 1
    if (blue_pacman_start == 1) {
      blue_pacman_start = 0;
      // green_pacman_start=1;
      blue_pacman_setup(&blue_pacman);
    }

    if (blue_pacman.direction == LEFT_UP) {
      switch (blue_pacman.row_left_upordown) {
      case 2 ... matrix_width - 8:
        game_graphics_packman(blue_pacman.row_left_upordown, blue_pacman.col_leftorrigt_up, blue_pacman.direction,
                              blue_pacman.packman_color, PLAYER_1);
        // led_matrix__update_display();
        blue_pacman.row_left_upordown++;
        break;
      case matrix_width - 7 ... matrix_width: // Check this condition
        var = rand();
        if (var % 2) {
          blue_pacman.direction = DOWN_LEFT;
        } else {
          blue_pacman.direction = RIGHT_UP;
        }
        blue_pacman.row_left_upordown = 2;
        break;
      }
    }
    if (blue_pacman.direction == RIGHT_UP) {
      switch (blue_pacman.row_right_upordown) {
      case 7 ... matrix_width - 3:
        game_graphics_packman(blue_pacman.row_right_upordown, blue_pacman.col_leftorrigt_up, blue_pacman.direction,
                              blue_pacman.packman_color, PLAYER_1);
        // led_matrix__update_display();
        blue_pacman.row_right_upordown--;
        break;
      case 0 ... 6: // Check this condition
        var = rand();
        if (var % 2) {
          blue_pacman.direction = LEFT_UP;
        } else {
          blue_pacman.direction = DOWN_RIGHT;
        }
        blue_pacman.row_right_upordown = matrix_width - 3;
        break;
      }
    }
    if (blue_pacman.direction == DOWN_LEFT) {
      switch (blue_pacman.col_down_leftorright) {
      case (matrix_height / 2) + 5 ... matrix_height - 5:
        game_graphics_packman(blue_pacman.row_upordown_left, blue_pacman.col_down_leftorright, blue_pacman.direction,
                              blue_pacman.packman_color, PLAYER_1);
        // led_matrix__update_display();
        blue_pacman.col_down_leftorright--;
        break;
      case 0 ...(matrix_height / 2) + 4: // Check this condition
        var = rand();
        if (var % 2) {
          blue_pacman.direction = RIGHT_DOWN;
        } else {
          blue_pacman.direction = UP_LEFT;
        }
        blue_pacman.col_down_leftorright = matrix_height - 5;
        break;
      }
    }
    if (blue_pacman.direction == UP_LEFT) {
      switch (blue_pacman.col_up_leftorright) {
      case (matrix_height / 2)... matrix_width - 10:
        game_graphics_packman(blue_pacman.row_upordown_left, blue_pacman.col_up_leftorright, blue_pacman.direction,
                              blue_pacman.packman_color, PLAYER_1);
        // led_matrix__update_display();
        blue_pacman.col_up_leftorright++;
        break;
      case matrix_height - 9 ... matrix_height:
        var = rand();
        if (var % 2) {
          blue_pacman.direction = RIGHT_UP;
        } else {
          blue_pacman.direction = DOWN_LEFT;
        }
        blue_pacman.col_up_leftorright = (matrix_height / 2);
        break;
      }
    }

    if (blue_pacman.direction == RIGHT_DOWN) {
      switch (blue_pacman.row_right_upordown) {
      case 7 ... matrix_height - 3:
        game_graphics_packman(blue_pacman.row_right_upordown, blue_pacman.col_leftorrigt_down, blue_pacman.direction,
                              blue_pacman.packman_color, PLAYER_1);
        // led_matrix__update_display();
        blue_pacman.row_right_upordown--;
        break;
      case 0 ... 6:
        var = rand();
        if (var % 2) {
          blue_pacman.direction = UP_RIGHT;
        } else {
          blue_pacman.direction = LEFT_DOWN;
        }
        blue_pacman.row_right_upordown = matrix_height - 3;
        break;
      }
    }
    if (blue_pacman.direction == LEFT_DOWN) {
      switch (blue_pacman.row_left_upordown) {
      case 2 ... matrix_width - 8:
        game_graphics_packman(blue_pacman.row_left_upordown, blue_pacman.col_leftorrigt_down, blue_pacman.direction,
                              blue_pacman.packman_color, PLAYER_1);
        // led_matrix__update_display();
        blue_pacman.row_left_upordown++;
        break;
      case matrix_width - 7 ... matrix_width:
        var = rand();
        if (var % 2) {
          blue_pacman.direction = UP_LEFT;
        } else {
          blue_pacman.direction = RIGHT_DOWN;
        }
        blue_pacman.row_left_upordown = 2;
        break;
      }
    }

    if (blue_pacman.direction == UP_RIGHT) {
      switch (blue_pacman.col_up_leftorright) {
      case (matrix_height / 2)... matrix_height - 10:
        game_graphics_packman(blue_pacman.row_upordown_right, blue_pacman.col_up_leftorright, blue_pacman.direction,
                              blue_pacman.packman_color, PLAYER_1);
        // led_matrix__update_display();
        blue_pacman.col_up_leftorright++;
        break;
      case matrix_height - 9 ... matrix_height:
        var = rand();
        if (var % 2) {
          blue_pacman.direction = LEFT_UP;
        } else {
          blue_pacman.direction = DOWN_RIGHT;
        }
        blue_pacman.col_up_leftorright = (matrix_height / 2) + 4;
        break;
      }
    }
    if (blue_pacman.direction == DOWN_RIGHT) {
      switch (blue_pacman.col_down_leftorright) {
      case (matrix_height / 2) + 5 ... matrix_height - 5:
        game_graphics_packman(blue_pacman.row_upordown_right, blue_pacman.col_down_leftorright, blue_pacman.direction,
                              blue_pacman.packman_color, PLAYER_1);
        // led_matrix__update_display();
        blue_pacman.col_down_leftorright--;
        break;
      case 0 ...(matrix_height / 2) + 4:
        var = rand();
        if (var % 2) {
          blue_pacman.direction = LEFT_DOWN;
        } else {
          blue_pacman.direction = UP_RIGHT;
        }
        blue_pacman.col_down_leftorright = matrix_height - 5;
        break;
      }
    }

    vTaskDelay(50);
  }
}

static void green_pacman_task(void *params) {
  pacman_s green_pacman;
  green_pacman.row_upordown_right = 5, green_pacman.row_upordown_left = matrix_width - 6,
  green_pacman.row_left_upordown = 2, green_pacman.row_right_upordown = matrix_width - 3;
  green_pacman.col_up_leftorright = 4, green_pacman.col_down_leftorright = (matrix_height / 2) - 1,
  green_pacman.col_leftorrigt_up = (matrix_height / 2) - 4, green_pacman.col_leftorrigt_down = 7;
  int var;
  led_matrix__color_e packman_color = GREEN;
  green_pacman.direction = LEFT_UP;

  while (true) {
    led_matrix__fill_frame_buffer_inside_grid_lower_half(); // Player 2
    if (green_pacman.direction == LEFT_UP) {
      switch (green_pacman.row_left_upordown) {
      case 2 ... matrix_width - 8:
        game_graphics_packman(green_pacman.row_left_upordown, green_pacman.col_leftorrigt_up, green_pacman.direction,
                              packman_color, PLAYER_2);
        // led_matrix__update_display();
        green_pacman.row_left_upordown++;
        break;
      case matrix_width - 7 ... matrix_width: // Check this condition
        var = rand();
        if (var % 2) {
          green_pacman.direction = DOWN_LEFT;
        } else {
          green_pacman.direction = RIGHT_UP;
        }
        green_pacman.row_left_upordown = 2;
        break;
      }
    }
    if (green_pacman.direction == RIGHT_UP) {
      switch (green_pacman.row_right_upordown) {
      case 7 ... matrix_width - 3:
        game_graphics_packman(green_pacman.row_right_upordown, green_pacman.col_leftorrigt_up, green_pacman.direction,
                              packman_color, PLAYER_2);
        // led_matrix__update_display();
        green_pacman.row_right_upordown--;
        break;
      case 0 ... 6: // Check this condition
        var = rand();
        if (var % 2) {
          green_pacman.direction = LEFT_UP;
        } else {
          green_pacman.direction = DOWN_RIGHT;
        }
        green_pacman.row_right_upordown = matrix_width - 3;
        break;
      }
    }
    if (green_pacman.direction == DOWN_LEFT) {
      switch (green_pacman.col_down_leftorright) {
      case 9 ...(matrix_height / 2) - 1:
        game_graphics_packman(green_pacman.row_upordown_left, green_pacman.col_down_leftorright, green_pacman.direction,
                              packman_color, PLAYER_2);
        // led_matrix__update_display();
        green_pacman.col_down_leftorright--;
        break;
      case 0 ... 8:
        var = rand();
        if (var % 2) {
          green_pacman.direction = RIGHT_DOWN;
        } else {
          green_pacman.direction = UP_LEFT;
        }
        green_pacman.col_down_leftorright = (matrix_height / 2) - 1;
        break;
      }
    }
    if (green_pacman.direction == UP_LEFT) {
      switch (green_pacman.col_up_leftorright) {
      case 4 ...(matrix_height / 2) - 6:
        game_graphics_packman(green_pacman.row_upordown_left, green_pacman.col_up_leftorright, green_pacman.direction,
                              packman_color, PLAYER_2);
        // led_matrix__update_display();
        green_pacman.col_up_leftorright++;
        break;
      case (matrix_height / 2) - 5 ... matrix_height:
        var = rand();
        if (var % 2) {
          green_pacman.direction = RIGHT_UP;
        } else {
          green_pacman.direction = DOWN_LEFT;
        }
        green_pacman.col_up_leftorright = 4;
        break;
      }
    }

    if (green_pacman.direction == RIGHT_DOWN) {
      switch (green_pacman.row_right_upordown) {
      case 7 ... matrix_height - 3:
        game_graphics_packman(green_pacman.row_right_upordown, green_pacman.col_leftorrigt_down, green_pacman.direction,
                              packman_color, PLAYER_2);
        // led_matrix__update_display();
        green_pacman.row_right_upordown--;
        break;
      case 0 ... 6:
        var = rand();
        if (var % 2) {
          green_pacman.direction = UP_RIGHT;
        } else {
          green_pacman.direction = LEFT_DOWN;
        }
        green_pacman.row_right_upordown = matrix_height - 3;
        break;
      }
    }
    if (green_pacman.direction == LEFT_DOWN) {
      switch (green_pacman.row_left_upordown) {
      case 2 ... matrix_width - 8:
        game_graphics_packman(green_pacman.row_left_upordown, green_pacman.col_leftorrigt_down, green_pacman.direction,
                              packman_color, PLAYER_2);
        // led_matrix__update_display();
        green_pacman.row_left_upordown++;
        break;
      case matrix_width - 7 ... matrix_width:
        var = rand();
        if (var % 2) {
          green_pacman.direction = UP_LEFT;
        } else {
          green_pacman.direction = RIGHT_DOWN;
        }
        green_pacman.row_left_upordown = 2;
        break;
      }
    }

    if (green_pacman.direction == UP_RIGHT) {
      switch (green_pacman.col_up_leftorright) {
      case 4 ...(matrix_height / 2) - 6:
        game_graphics_packman(green_pacman.row_upordown_right, green_pacman.col_up_leftorright, green_pacman.direction,
                              packman_color, PLAYER_2);
        // led_matrix__update_display();
        green_pacman.col_up_leftorright++;
        break;
      case (matrix_height / 2) - 5 ... matrix_height:
        var = rand();
        if (var % 2) {
          green_pacman.direction = LEFT_UP;
        } else {
          green_pacman.direction = DOWN_RIGHT;
        }
        green_pacman.col_up_leftorright = 4;
        break;
      }
    }
    if (green_pacman.direction == DOWN_RIGHT) {
      switch (green_pacman.col_down_leftorright) {
      case 9 ...(matrix_height / 2) - 1:
        game_graphics_packman(green_pacman.row_upordown_right, green_pacman.col_down_leftorright,
                              green_pacman.direction, packman_color,
                              PLAYER_2); // led_matrix__update_display();
        green_pacman.col_down_leftorright--;
        break;
      case 0 ... 8:
        var = rand();
        if (var % 2) {
          green_pacman.direction = LEFT_DOWN;
        } else {
          green_pacman.direction = UP_RIGHT;
        }
        green_pacman.col_down_leftorright = (matrix_height / 2) - 1;
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