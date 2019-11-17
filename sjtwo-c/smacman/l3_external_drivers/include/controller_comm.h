/**
 * @file
 * UART Communication between master board and two
 * controller boards.
 *
 * Thread Safety Assessment:
 * <Not thread safe because>
 * <Thread safe by using xyz design>
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define CONTROLLER_COMM__USING_ACCEL_FILTER
// #define CONTROLLER_COMM__CHECKSUM

#include "uart.h"
#include "gpio.h"
#ifdef CONTROLLER_COMM__USING_ACCEL_FILTER
    #include "accel_filter.h"
#else
    #include "acceleration.h"
#endif
#include "stdio.h"
#include "stdbool.h"
#include "queue.h"
#include "portmacro.h"

/*******************************************************************************
 *
 *                               D E F I N E S
 *
 ******************************************************************************/
const static uint32_t controller_comm__uart_baud_rate = 115200;
const static uint8_t controller_comm__rx_queue_size = 10;
const static uint8_t controller_comm__tx_queue_size = 10;
#ifdef CONTROLLER_COMM__CHECKSUM
const static uint32_t controller_comm__message_receive_timeout_ms = 10;
#endif
/*******************************************************************************
 *
 *                                 E N U M S
 *
 ******************************************************************************/
typedef enum {
    CONTROLLER_COMM__ROLE_MASTER = 1,
    CONTROLLER_COMM__ROLE_PLAYER_1,
    CONTROLLER_COMM__ROLE_PLAYER_2
} controller_comm__role_e;


typedef enum {
    CONTROLLER_COMM__MESSAGE_TYPE_REQUEST_ACCEL_VAL,
    CONTROLLER_COMM__MESSAGE_TYPE_SEND_ACCEL_VAL,
    CONTROLLER_COMM__MESSAGE_TYPE_UPDATE_SCORE,
} controller_comm__message_type_e;

/*******************************************************************************
 *
 *                               T Y P E D E F S
 *
 ******************************************************************************/

typedef struct {
    controller_comm__role_e role;
    uart_e uart;
    QueueHandle_t rx_queue;
    QueueHandle_t tx_queue;
} controller_comm_s;

typedef struct {
    controller_comm__role_e recipient;
    controller_comm__role_e sender;
    controller_comm__message_type_e message_type;
    uint16_t data;
#ifdef CONTROLLER_COMM__CHECKSUM
    uint16_t checksum;
#endif
} controller_comm__message_s;

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/
controller_comm_s controller_comm__init(controller_comm__role_e role, uart_e uart);
void controller_comm__freertos_task(void *controller_comm_struct);

#ifdef __cplusplus
} /* extern "C" */
#endif
