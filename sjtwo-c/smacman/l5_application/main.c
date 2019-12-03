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

extern volatile bool collided;
static bool green_pacman_start = 0;
static bool blue_pacman_start = 0;
static void smacman__startup(void);

static void display_task(void *params);
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
  smacman__startup();
  acceleration__axis_data_s sensor_avg_value;
  sensor_avg_value = acceleration__get_data();
  srand(sensor_avg_value.x + sensor_avg_value.z);
  // xTaskCreate(blue_pacman_task, "blue_pacman", (2048U / sizeof(void *)), NULL, PRIORITY_LOW, NULL);
  // xTaskCreate(green_pacman_task, "green_pacman", (2048U / sizeof(void *)), NULL, PRIORITY_LOW, NULL);
  // xTaskCreate(blue_paddle_task, "paddle_blue", (2048U / sizeof(void *)), NULL, PRIORITY_MEDIUM, NULL);
  // xTaskCreate(green_paddle_task, "paddle_green", (2048U / sizeof(void *)), NULL, PRIORITY_MEDIUM, NULL);
  // xTaskCreate(ball_task, "ball", (2048U / sizeof(void *)), NULL, PRIORITY_HIGH, NULL);
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

// static void master_task(void *params) { int state = (int)*params; }

static void display_task(void *params) {
  led_matrix__displayGridBorders(PINK);
  led_matrix__display_intro_GridBorder();
  led_matrix__display_intro_Pacs();
  while (true) {
    led_matrix__update_display();
    vTaskDelay(5);
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