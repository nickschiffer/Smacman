#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "board_io.h"
#include "common_macros.h"
#include "gpio.h"
#include "uart.h"
#include "sj2_cli.h"
#include "controller_comm.h"

#define PLAYER_1
//#define MASTER

static controller_comm_s controller;
static gpio_s gpio_tx, gpio_rx;

int main(void) {

// gpio_tx = gpio__construct_with_function(GPIO__PORT_4, 28,GPIO__FUNCTION_2);
// gpio_rx = gpio__construct_with_function(GPIO__PORT_4, 29,GPIO__FUNCTION_2);

gpio_tx = gpio__construct_as_output(GPIO__PORT_4, 28);
gpio_rx = gpio__construct_as_input(GPIO__PORT_4, 29);

// gpio__set_function(gpio_rx, GPIO__FUNCTION_2);
// gpio__set_function(gpio_tx, GPIO__FUNCTION_2);

// uart_lab__init(UART__3, b115200, UART_WLS_8_BIT);
// uart__enable_receive_interrupt(labUART__3);
// uart__enable_receive_interrupt(labUART__3);
// gpio_tx = gpio__construct_with_function(GPIO__PORT_4, 28,GPIO__FUNCTION_2);
// gpio_rx = gpio__construct_with_function(GPIO__PORT_4, 29,GPIO__FUNCTION_2);

#ifdef PLAYER_1
  controller = controller_comm__init(CONTROLLER_COMM__ROLE_PLAYER_1, UART__3, gpio_tx, gpio_rx);
#endif
#ifdef MASTER
  controller = controller_comm__init(CONTROLLER_COMM__ROLE_MASTER, UART__3, gpio_tx, gpio_rx);
#endif

xTaskCreate(controller_comm__freertos_task, "controller", (10000U / sizeof(void *)), (void *) &controller, PRIORITY_LOW, NULL);  

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
