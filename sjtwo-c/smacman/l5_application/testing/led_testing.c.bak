#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "board_io.h"
#include "common_macros.h"
#include "gpio.h"
#include "sj2_cli.h"

#include "led_graphics.h"
#include "led_matrix.h"

static void display_task(void *params);
static gpio_s switch_plus, switch_minus;

int main(void) {
  switch_plus = gpio__construct_as_input(GPIO__PORT_1, 10);  // SW3
  switch_minus = gpio__construct_as_input(GPIO__PORT_1, 15); // SW2
  gpio__set_as_input(switch_plus);
  gpio__set_as_input(switch_minus);
  led_matrix__init();
  // displayGridBorders(Lime);
  // drawPackMan(32, 32);
  xTaskCreate(display_task, "led0", (2048U / sizeof(void *)), NULL, PRIORITY_HIGH, NULL);

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

static void display_task(void *params) {
  int i = 32;
  while (true) {
    // if (i % 2 == 0)
    //   drawPackMan(11, 31);
    // else
    //   drawPackMan(43, 31);
    // updateDisplay();
    // vTaskDelay(1);
    // i++;

    if (!gpio__get(switch_plus)) {
      i++;
    } else if (!gpio__get(switch_minus)) {
      i--;
    }
    if (i > 60) {
      i = 10;
    }
    drawPackMan(i - 3, i);
    led_matrix__update_display();
    vTaskDelay(5);
  }
}
