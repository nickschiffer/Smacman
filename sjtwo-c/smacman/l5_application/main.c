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
#include "smacman_common.h"

typedef struct {
  TaskHandle_t blue_pac;
  TaskHandle_t green_pac;
} main__pac_task_s;

xSemaphoreHandle mutex_for_spi;
xSemaphoreHandle signal_for_pacman_suspended;
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
static void main__blue_packman_task_with_all_levels(void *params);
static void main__pacman_all_levels_task_init(TaskHandle_t *xPacmanHandle);

static BaseType_t create_task_game(void *function_name, char *task_name, uint32_t stack_size, void *p, uint8_t priority,
                                   TaskHandle_t *xTaskHandle, bool is_suspended);

void controller_gpio_init();
static void _test_pause_timer(void *params);

gpio_s led0, led1, led2, led3;
gpio_s switch0, switch1, switch2, switch3;
paddle_s blue_paddle, green_paddle;
pacman_s blue_pacman_init_level1, green_pacman_init_level1, blue_pacman_init_level2, green_pacman_init_level2;
// int ball_level;
QueueHandle_t state_main_queue, state_pacman_pause_queue;
ball_param ball_level_queue;

main__pac_task_s pacman_tasks;

int main(void) {
  state_main_queue = xQueueCreate(1, sizeof(game_logic_game_state_s));
  state_pacman_pause_queue = xQueueCreate(1, sizeof(bool));
  blue_paddle_direction_queue = xQueueCreate(1, sizeof(paddle_direction_e));
  green_paddle_direction_queue = xQueueCreate(1, sizeof(paddle_direction_e));

  ball_level_queue.state_queue = &state_main_queue;
  smacman__startup();
  // mutex_for_spi = xSemaphoreCreateMutex();
  mutex_for_spi = xSemaphoreCreateBinary();
  signal_for_pacman_suspended = xSemaphoreCreateBinary();
  // ssp2__initialize(24 * 1000);

  acceleration__axis_data_s sensor_avg_value;
  sensor_avg_value = acceleration__get_data();
  srand(sensor_avg_value.x + sensor_avg_value.z);
#if SMACMAN_CONTROLLER_CONNECTED
  controller = controller_comm__init(CONTROLLER_COMM__ROLE_MASTER, UART__3, gpio_tx, gpio_rx);

  xTaskCreate(controller_comm__freertos_task, "controller", (5000U / sizeof(void *)), (void *)&controller, PRIORITY_LOW,
              NULL);
#else
  xTaskCreate(_test_pause_timer, "for_pause", (1024U / sizeof(void *)), NULL, PRIORITY_MEDIUM, NULL);

#endif
  xTaskCreate(master_task, "master_task", (4096U / sizeof(void *)), NULL, PRIORITY_CRITICAL, NULL);
  xTaskCreate(display_task, "Display_Task", (1024U / sizeof(void *)), NULL, PRIORITY_HIGH, NULL);

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

void check_score(pacman_level_e *which_pacman_level, TaskHandle_t *xPacmanHandle) {
  uint16_t score_now = max_get_max_score();
  SMACMAN__DEBUG_PRINTF("Score_now = %i", score_now);
  game_logic_game_state_s temp_game_current_state_send;
  if ((score_now > 0) && (score_now <= 33) && (*which_pacman_level == PACMAN_LEVEL_3)) {
    vTaskSuspend(pacman_tasks.blue_pac);
    vTaskSuspend(pacman_tasks.green_pac);
    pacman_tasks.blue_pac = xPacmanHandle[blue_pacman_l1];
    pacman_tasks.green_pac = xPacmanHandle[green_pacman_l1];
    ball_level_queue.level = 1;
    *which_pacman_level = PACMAN_LEVEL_1;
    temp_game_current_state_send = IN_PROGRESS_STATE;
    xQueueSend(*(ball_level_queue.state_queue), &temp_game_current_state_send, portMAX_DELAY);

  } else if ((score_now > 33) && (score_now < 66) && (*which_pacman_level == PACMAN_LEVEL_1)) {
    vTaskSuspend(pacman_tasks.blue_pac);
    vTaskSuspend(pacman_tasks.green_pac);
    pacman_tasks.blue_pac = xPacmanHandle[blue_pacman_l2];
    pacman_tasks.green_pac = xPacmanHandle[green_pacman_l2];
    ball_level_queue.level = 2;
    *which_pacman_level = PACMAN_LEVEL_2;
    temp_game_current_state_send = IN_PROGRESS_STATE;
    xQueueSend(*(ball_level_queue.state_queue), &temp_game_current_state_send, portMAX_DELAY);
  } else if ((score_now > 33) && (score_now < 66) && (*which_pacman_level == PACMAN_LEVEL_2)) {
    vTaskSuspend(pacman_tasks.blue_pac);
    vTaskSuspend(pacman_tasks.green_pac);
    pacman_tasks.blue_pac = xPacmanHandle[blue_pacman_l2];
    pacman_tasks.green_pac = xPacmanHandle[green_pacman_l2];
    ball_level_queue.level = 3;
    *which_pacman_level = PACMAN_LEVEL_3;
    temp_game_current_state_send = IN_PROGRESS_STATE;
    xQueueSend(*(ball_level_queue.state_queue), &temp_game_current_state_send, portMAX_DELAY);
  }
}

static void master_task(void *params) {
  set_game_state(INIT_STATE);
  game_logic_game_state_s game_current_state;                        // INIT_STATE;
  game_logic_game_state_s temp_game_current_state_send = INIT_STATE; // INIT_STATE;
  BaseType_t xReturned;
  TaskHandle_t xHandle[6] = {NULL};
  int score_now = -1;

  pacman_level_e which_pacman_level = PACMAN_LEVEL_1;
  TaskHandle_t xPacmanHandle[6] = {NULL};

  xQueueSend(*(ball_level_queue.state_queue), &temp_game_current_state_send, portMAX_DELAY);
  main__pacman_all_levels_task_init(&xPacmanHandle);
  pacman_tasks.blue_pac = xPacmanHandle[blue_pacman_l1];
  pacman_tasks.green_pac = xPacmanHandle[green_pacman_l1];
  while (1) {
    check_score(&which_pacman_level, xPacmanHandle);

    xQueueReceive(*(ball_level_queue.state_queue), &game_current_state, portMAX_DELAY);
    // fprintf(stderr, "Game Current State = %d\n", game_current_state);
    // get_game_state();
    switch (game_current_state) {
    case INIT_STATE:
      common__splash_screen(); // WIll Show splash screen in the start;
      // xReturned = create_task_game(pacman_level_task[which_pacman_level], "blue_pacman", 2048,
      // &blue_pacman_init_level2,
      //  PRIORITY_LOW, &xHandle[blue_pacman], TASK_SUSPENDED);
      xReturned = create_task_game(main__blue_packman_task_with_all_levels, "blue_pacman", 2048,
                                   &blue_pacman_init_level1, PRIORITY_LOW, &xHandle[blue_pacman], TASK_SUSPENDED);
      // xReturned = create_task_game(pacman_level_task[which_pacman_level], "green_pacman", 2048,
      //                              &green_pacman_init_level1, PRIORITY_LOW, &xHandle[green_pacman], TASK_SUSPENDED);
      xReturned = create_task_game(paddle_task, "paddle_blue", 2048, &blue_paddle, PRIORITY_MEDIUM,
                                   &xHandle[paddle_blue], TASK_SUSPENDED);
      xReturned = create_task_game(paddle_task, "paddle_green", 2048, &green_paddle, PRIORITY_MEDIUM,
                                   &xHandle[paddle_green], TASK_SUSPENDED);
      xReturned =
          create_task_game(ball_task, "ball", 2048, &ball_level_queue, PRIORITY_HIGH, &xHandle[ball], TASK_SUSPENDED);

      xReturned = create_task_game(score_task, "players_score", 2048, NULL, PRIORITY_MEDIUM, &xHandle[players_score],
                                   TASK_NOT_SUSPENDED);

#if SMACMAN_CONTROLLER_CONNECTED
#else
      vTaskDelay(5000);
#endif
      set_game_state(IN_PROGRESS_STATE);
#if SMACMAN_CONTROLLER_CONNECTED
#else
      temp_game_current_state_send = IN_PROGRESS_STATE;
      xQueueSend(*(ball_level_queue.state_queue), &temp_game_current_state_send, portMAX_DELAY);
      score_now = 0;
#endif
      // game_current_state = IN_PROGRESS_STATE;
      break;
    case IN_PROGRESS_STATE:
      // SMACMAN__DEBUG_PRINTF("")
      fprintf(stderr, "In progress state\n");
      vTaskSuspend(xHandle[players_score]);
      vTaskResume(pacman_tasks.blue_pac);
      vTaskResume(pacman_tasks.green_pac);
      // vTaskResume(xHandle[blue_pacman]);
      // vTaskResume(xHandle[green_pacman]);
      vTaskResume(xHandle[paddle_blue]);
      vTaskResume(xHandle[paddle_green]);
      vTaskResume(xHandle[ball]);
      break;
    case IN_PAUSE_STATE:
      fprintf(stderr, "In Pause state\n");
      // vTaskSuspend(xHandle[blue_pacman]);
      vTaskSuspend(pacman_tasks.blue_pac);
      vTaskSuspend(pacman_tasks.green_pac);
      // vTaskSuspend(xHandle[green_pacman]);
      vTaskSuspend(xHandle[paddle_blue]);
      vTaskSuspend(xHandle[paddle_green]);
      vTaskSuspend(xHandle[ball]);
      led_matrix_clear_frame_buffer_inside_grid(0x3FFFFFFFFFFFFFFC);
      vTaskResume(xHandle[players_score]);

      // vTaskDelay(3000);
      set_game_state(IN_PAUSE_STATE);
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
#if SMACMAN_CONTROLLER_CONNECTED
    (void)controller_poll_ready_or_pause_and_take_action(ball_level_queue.state_queue);
#endif
    led_matrix__update_display();
    vTaskDelay(5);
  }
}

void controller_gpio_init() {
  gpio_tx = gpio__construct_as_output(GPIO__PORT_4, 28);
  gpio_rx = gpio__construct_as_input(GPIO__PORT_4, 29);
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
#if SMACMAN_CONTROLLER_CONNECTED
  controller_gpio_init();
#endif
}

static void main__pacman_all_levels_task_init(TaskHandle_t *xPacmanHandle) {
  BaseType_t xReturned;
  xReturned = create_task_game(pacman_level_task[PACMAN_LEVEL_1], "blue_pacman_1", 2048, &blue_pacman_init_level1,
                               PRIORITY_LOW, &xPacmanHandle[blue_pacman_l1], TASK_SUSPENDED);
  xReturned = create_task_game(pacman_level_task[PACMAN_LEVEL_1], "green_pacman", 2048, &green_pacman_init_level1,
                               PRIORITY_LOW, &xPacmanHandle[green_pacman_l1], TASK_SUSPENDED);

  xReturned = create_task_game(pacman_level_task[PACMAN_LEVEL_2], "blue_pacman", 2048, &blue_pacman_init_level2,
                               PRIORITY_LOW, &xPacmanHandle[blue_pacman_l2], TASK_SUSPENDED);
  xReturned = create_task_game(pacman_level_task[PACMAN_LEVEL_2], "green_pacman", 2048, &green_pacman_init_level2,
                               PRIORITY_LOW, &xPacmanHandle[green_pacman_l2], TASK_SUSPENDED);

  xReturned = create_task_game(pacman_level_task[PACMAN_LEVEL_3], "blue_pacman", 2048, &blue_pacman_init_level2,
                               PRIORITY_LOW, &xPacmanHandle[blue_pacman_l3], TASK_SUSPENDED);
  xReturned = create_task_game(pacman_level_task[PACMAN_LEVEL_3], "green_pacman", 2048, &green_pacman_init_level2,
                               PRIORITY_LOW, &xPacmanHandle[green_pacman_l3], TASK_SUSPENDED);
}

static void main__blue_packman_task_with_all_levels(void *params) {
  int score = 0;
  bool state;
  TaskHandle_t xPacmanHandle[6] = {NULL};
  main__pacman_all_levels_task_init(&xPacmanHandle);

  while (1) {
    pacman_tasks.blue_pac = xPacmanHandle[blue_pacman_l1];
    pacman_tasks.green_pac = xPacmanHandle[green_pacman_l1];

    // if(score >0){

    // }

    // pacman_task_level1(&blue_pacman_init_level1);
    // vTaskDelay(50);
  }
}

static void _test_pause_timer(void *params) {
  game_logic_game_state_s temp_game_current_state_send;
  vTaskDelay(5500);
  while (1) {
    printf("in pause\n");
    temp_game_current_state_send = IN_PAUSE_STATE;
    xQueueSend(*(ball_level_queue.state_queue), &temp_game_current_state_send, portMAX_DELAY);
    vTaskDelay(2000);
    temp_game_current_state_send = IN_PROGRESS_STATE;
    xQueueSend(*(ball_level_queue.state_queue), &temp_game_current_state_send, portMAX_DELAY);
    vTaskDelay(5000);
  }
}