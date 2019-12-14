#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <time.h>

#include "acceleration.h"
#include "board_io.h"
#include "common_macros.h"
#include "gpio.h"
#include "sj2_cli.h"

#include "controller_comm.h"
#include "game_logic.h"
#include "mp3.h"
#include "queue.h"
#include "semphr.h"

xSemaphoreHandle mutex_for_spi;
static controller_comm_s controller;
static gpio_s gpio_tx, gpio_rx;
extern volatile bool collided;
static bool green_pacman_start = 0;
static bool blue_pacman_start = 0;

static void smacman__startup(void);

static void master_task(void *params);
// static void blue_pacman_task(void *params);
// static void green_pacman_task(void *params);
// static void blue_paddle_task(void *params);
// static void green_paddle_task(void *params);
// static void ball_task(void *params);
static void display_task(void *params);

static BaseType_t create_task_game(void *function_name, char *task_name, uint32_t stack_size, void *p, uint8_t priority,
                                   TaskHandle_t *xTaskHandle, bool is_suspended);

gpio_s led0, led1, led2, led3;
gpio_s switch0, switch1, switch2, switch3;
paddle_s blue_paddle, green_paddle;
pacman_s blue_pacman_init_level1, green_pacman_init_level1, blue_pacman_init_level2, green_pacman_init_level2;
// int ball_level;
QueueHandle_t state_main_queue;
ball_param ball_level_queue;

static void file_read(void *params) {
  // open_file("song.mp3", &fp);
  while (1) {
    // if (xSemaphoreTake(mutex_for_spi, 1000)) {
    if (!mp3_read_from_sd()) {
      fprintf(stderr, "Failed Sleeping indefinetly\n");
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

void controller_gpio_init() {
  gpio_tx = gpio__construct_as_output(GPIO__PORT_4, 28);
  gpio_rx = gpio__construct_as_input(GPIO__PORT_4, 29);
}

int main(void) {
  state_main_queue = xQueueCreate(1, sizeof(game_logic_game_state_s));
  ball_level_queue.state_queue = &state_main_queue;
  smacman__startup();
  controller_gpio_init();
  // mutex_for_spi = xSemaphoreCreateMutex();
  mutex_for_spi = xSemaphoreCreateBinary();
  // ssp2__initialize(24 * 1000);

  acceleration__axis_data_s sensor_avg_value;
  sensor_avg_value = acceleration__get_data();
  srand(sensor_avg_value.x + sensor_avg_value.z);

  controller = controller_comm__init(CONTROLLER_COMM__ROLE_MASTER, UART__3, gpio_tx, gpio_rx);

  xTaskCreate(controller_comm__freertos_task, "controller", (5000U / sizeof(void *)), (void *)&controller, PRIORITY_LOW,
              NULL);

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

static BaseType_t create_task_game(void *function_name, char *task_name, uint32_t stack_size, void *p, uint8_t priority,
                                   TaskHandle_t *xTaskHandle, bool is_suspended) {
  BaseType_t xReturned;
  xReturned = xTaskCreate(function_name, task_name, (stack_size / sizeof(void *)), p, priority, xTaskHandle);
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
  set_game_state(INIT_STATE);
  game_logic_game_state_s game_current_state;                        // INIT_STATE;
  game_logic_game_state_s temp_game_current_state_send = INIT_STATE; // INIT_STATE;
  BaseType_t xReturned;
  TaskHandle_t xHandle[6] = {NULL};

  xQueueSend(*(ball_level_queue.state_queue), &temp_game_current_state_send, portMAX_DELAY);

  while (1) {
    xQueueReceive(*(ball_level_queue.state_queue), &game_current_state, portMAX_DELAY);
    // fprintf(stderr, "Game Current State = %d\n", game_current_state);
    // get_game_state();
    switch (game_current_state) {
    case INIT_STATE:
      xReturned = create_task_game(pacman_task_level2, "blue_pacman", 2048, &blue_pacman_init_level2, PRIORITY_LOW,
                                   &xHandle[blue_pacman], TASK_SUSPENDED);
      xReturned = create_task_game(pacman_task_level2, "green_pacman", 2048, &green_pacman_init_level2, PRIORITY_LOW,
                                   &xHandle[green_pacman], TASK_SUSPENDED);
      xReturned = create_task_game(paddle_task, "paddle_blue", 2048, &blue_paddle, PRIORITY_MEDIUM,
                                   &xHandle[paddle_blue], TASK_SUSPENDED);
      xReturned = create_task_game(paddle_task, "paddle_green", 2048, &green_paddle, PRIORITY_MEDIUM,
                                   &xHandle[paddle_green], TASK_SUSPENDED);
      xReturned =
          create_task_game(ball_task, "ball", 2048, &ball_level_queue, PRIORITY_HIGH, &xHandle[ball], TASK_SUSPENDED);

      xReturned = create_task_game(score_task, "players_score", 2048, NULL, PRIORITY_MEDIUM, &xHandle[players_score],
                                   TASK_NOT_SUSPENDED);
      set_game_state(IN_PROGRESS_STATE);
      temp_game_current_state_send = IN_PROGRESS_STATE;
      xQueueSend(*(ball_level_queue.state_queue), &temp_game_current_state_send, portMAX_DELAY);
      // game_current_state = IN_PROGRESS_STATE;
      break;
    case IN_PROGRESS_STATE:
      vTaskSuspend(xHandle[players_score]);
      vTaskResume(xHandle[blue_pacman]);
      vTaskResume(xHandle[green_pacman]);
      vTaskResume(xHandle[paddle_blue]);
      vTaskResume(xHandle[paddle_green]);
      vTaskResume(xHandle[ball]);
      break;
    case IN_SCORE_STATE:
      vTaskSuspend(xHandle[blue_pacman]);
      vTaskSuspend(xHandle[green_pacman]);
      vTaskSuspend(xHandle[paddle_blue]);
      vTaskSuspend(xHandle[paddle_green]);
      vTaskSuspend(xHandle[ball]);
      led_matrix_clear_frame_buffer_inside_grid(0x0FFFFFFFFFFFFFF0);
      vTaskResume(xHandle[players_score]);
      vTaskDelay(3000);
      set_game_state(IN_PROGRESS_STATE);
      led_matrix_clear_frame_buffer_inside_grid(0x0FFFFFFFFFFFFFF0);
      break;
    //
    default:
      SMACMAN__DEBUG_PRINTF("Game in progress\n");
    }
    vTaskDelay(500);
  }
}

static void display_task(void *params) {
  led_matrix__displayGridBorders(PINK);

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
  ball_level_queue.level = 2;
  blue_paddle = (paddle_s){matrix_width / 4, 2, BLUE};
  green_paddle = (paddle_s){matrix_width / 4, matrix_height - 4, GREEN};
  blue_pacman_init_level1 = (pacman_s){5,
                                       matrix_width - 6,
                                       2,
                                       matrix_width - 3,
                                       (matrix_height / 2),
                                       matrix_height - 5,
                                       matrix_height - 8,
                                       (matrix_height / 2) + 3,
                                       BLUE,
                                       LEFT_UP,
                                       1};
  green_pacman_init_level1 =
      (pacman_s){5,     matrix_width - 6, 2, matrix_width - 3, 4, (matrix_height / 2) - 1, (matrix_height / 2) - 4, 7,
                 GREEN, RIGHT_UP,         1};

  blue_pacman_init_level2 = (pacman_s){5,
                                       matrix_width - 6,
                                       2,
                                       matrix_width - 3,
                                       (matrix_height / 2),
                                       matrix_height - 5,
                                       (3 * (matrix_height / 4)) - 4,
                                       (matrix_height / 2) + 3,
                                       BLUE,
                                       LEFT_UP,
                                       2};
  green_pacman_init_level2 =
      (pacman_s){5,     matrix_width - 6, 2, matrix_width - 3, 4, (matrix_height / 2) - 1, (matrix_height / 4) + 4, 7,
                 GREEN, RIGHT_UP,         2};
  gpio__set(led0);
  gpio__set(led1);
  gpio__set(led2);
  gpio__set(led3);

  led_matrix__init();
}
