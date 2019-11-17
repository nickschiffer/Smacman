#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <time.h>

#include "board_io.h"
#include "common_macros.h"
#include "gpio.h"
#include "sj2_cli.h"

#include "led_graphics.h"
#include "led_matrix.h"

static void blue_pacman_task(void *params);
static void green_pacman_task(void *params);
static void blue_paddle_task(void *params);
static void green_paddle_task(void *params);
static void ball_task(void *params);
static void display_task(void *params);
static void ball_task(void *params);

typedef struct {
  int row;
  int col;
  int vx;
  int vy;
} ball_s;

static gpio_s led0, led1, led2, led3;
// static ball_s ball;

int main(void) {
  led0 = gpio__construct_as_output(GPIO__PORT_1, 18);
  led1 = gpio__construct_as_output(GPIO__PORT_1, 24);
  led2 = gpio__construct_as_output(GPIO__PORT_1, 26);
  led3 = gpio__construct_as_output(GPIO__PORT_2, 3);
  gpio__set(led0);
  gpio__set(led1);
  gpio__set(led2);
  gpio__set(led3);
  led_matrix__init();
  time_t t;
  srand(time(&t));
  xTaskCreate(blue_pacman_task, "blue_pacman", (2048U / sizeof(void *)), NULL, PRIORITY_MEDIUM, NULL);
  xTaskCreate(green_pacman_task, "green_pacman", (2048U / sizeof(void *)), NULL, PRIORITY_MEDIUM, NULL);
  xTaskCreate(blue_paddle_task, "paddle_blue", (2048U / sizeof(void *)), NULL, PRIORITY_MEDIUM, NULL);
  xTaskCreate(green_paddle_task, "paddle_green", (2048U / sizeof(void *)), NULL, PRIORITY_MEDIUM, NULL);
  xTaskCreate(ball_task, "ball", (2048U / sizeof(void *)), NULL, PRIORITY_HIGH, NULL);
  xTaskCreate(display_task, "startup", (1024U / sizeof(void *)), NULL, PRIORITY_HIGH, NULL);

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

// Blue paddle - at the bottom. Directions: LEFT_DOWN, RIGHT_DOWN
// Length of the paddle is 16
static void blue_paddle_task(void *params) {
  int row_right = matrix_width - 3, row_left = 2;
  int col = 2;
  led_matrix__direction_e direction = LEFT_DOWN;
  while (true) {
    if (direction == LEFT_DOWN) {
      switch (row_left) {
      case 2 ... matrix_width - 19:
        led_matrix__drawPaddle_blue(row_left, col, direction);
        row_left++;
        break;
      case matrix_width - 18 ... matrix_width: // Check this condition
        direction = RIGHT_DOWN;
        row_left = 2;
        break;
      }
    }
    if (direction == RIGHT_DOWN) {
      switch (row_right) {
      case 17 ... matrix_width - 3:
        led_matrix__drawPaddle_blue(row_right, col, direction);
        row_right--;
        break;
      case 0 ... 16: // Check this condition
        direction = LEFT_DOWN;
        row_right = matrix_width - 3;
        break;
      }
    }

    vTaskDelay(30);
  }
}

// Green paddle - at the top. Directions: LEFT_UP, RIGHT_UP
// Length of the paddle is 16
static void green_paddle_task(void *params) {
  int row_right = matrix_width - 3, row_left = 2;
  int col = matrix_height - 3;
  led_matrix__direction_e direction = LEFT_UP;
  while (true) {
    //
    if (direction == LEFT_UP) {
      switch (row_left) {
      case 2 ... matrix_width - 19:
        led_matrix__drawPaddle_green(row_left, col, direction);
        row_left++;
        break;
      case matrix_width - 18 ... matrix_width:
        direction = RIGHT_UP;
        row_left = 2;
        break;
      }
    }
    if (direction == RIGHT_UP) {
      switch (row_right) {
      case 17 ... matrix_width - 3:
        led_matrix__drawPaddle_green(row_right, col, direction);
        row_right--;
        break;
      case 0 ... 16:
        direction = LEFT_UP;
        row_right = matrix_width - 3;
        break;
      }
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
    led_matrix__fill_frame_buffer_inside_grid_upper_half();
    if (direction == LEFT_UP) {
      switch (row_left_upordown) {
      case 2 ... matrix_width - 8:
        led_matrix__drawPackMan(row_left_upordown, col_leftorrigt_up, direction, packman_color);
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
        led_matrix__drawPackMan(row_right_upordown, col_leftorrigt_up, direction, packman_color);
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
        led_matrix__drawPackMan(row_upordown_left, col_down_leftorright, direction, packman_color);
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
        led_matrix__drawPackMan(row_upordown_left, col_up_leftorright, direction, packman_color);
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
        led_matrix__drawPackMan(row_right_upordown, col_leftorrigt_down, direction, packman_color);
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
        led_matrix__drawPackMan(row_left_upordown, col_leftorrigt_down, direction, packman_color);
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
        led_matrix__drawPackMan(row_upordown_right, col_up_leftorright, direction, packman_color);
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
        led_matrix__drawPackMan(row_upordown_right, col_down_leftorright, direction, packman_color);
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

    vTaskDelay(100);
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
    led_matrix__fill_frame_buffer_inside_grid_lower_half();
    if (direction == LEFT_UP) {
      switch (row_left_upordown) {
      case 2 ... matrix_width - 8:
        led_matrix__drawPackMan(row_left_upordown, col_leftorrigt_up, direction, packman_color);
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
        led_matrix__drawPackMan(row_right_upordown, col_leftorrigt_up, direction, packman_color);
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
        led_matrix__drawPackMan(row_upordown_left, col_down_leftorright, direction, packman_color);
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
        led_matrix__drawPackMan(row_upordown_left, col_up_leftorright, direction, packman_color);
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
        led_matrix__drawPackMan(row_right_upordown, col_leftorrigt_down, direction, packman_color);
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
        led_matrix__drawPackMan(row_left_upordown, col_leftorrigt_down, direction, packman_color);
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
        led_matrix__drawPackMan(row_upordown_right, col_up_leftorright, direction, packman_color);
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
        led_matrix__drawPackMan(row_upordown_right, col_down_leftorright, direction, packman_color);
        // led_matrix__update_display();
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

    vTaskDelay(100);
  }
}
