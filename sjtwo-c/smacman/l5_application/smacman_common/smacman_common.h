#pragma once

#include "FreeRTOS.h"

#include "common_macros.h"
#include "gpio.h"
#include "sj2_cli.h"
#include <stdio.h>

#include "controller_comm.h"
#include "queue.h"

#define SMACMAN__ENABLE_DEBUGGING 1
#define SMACMAN_CONTROLLER_CONNECTED 1

#if SMACMAN__ENABLE_DEBUGGING
#include <stdio.h>
#define SMACMAN__DEBUG_PRINTF(f_, ...)                                                                                 \
  do {                                                                                                                 \
    fprintf(stderr, "SMACMAN:");                                                                                       \
    fprintf(stderr, (f_), ##__VA_ARGS__);                                                                              \
    fprintf(stderr, "\n");                                                                                             \
  } while (0)
#else
#define SMACMAN__DEBUG_PRINTF(f_, ...) /* NOOP */
#endif

void common__splash_screen();

void controller_poll_ready_or_pause_and_take_action(QueueHandle_t *state_queue);