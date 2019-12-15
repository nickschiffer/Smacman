/**
 * @file
 *
 * Provides a filtered accelerometer position
 *
 * Thread Safety Assessment:
 * <Not thread safe because>
 * <Thread safe by using xyz design>
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "acceleration.h"
#include "assert.h"
#include "task.h"

/*******************************************************************************
 *
 *                               D E F I N E S
 *
 ******************************************************************************/
const static uint32_t accel_filter__intersample_delay_ms = 10;
const static uint32_t accel_filter__sampling_interval_ms = 100;
const static uint32_t accel_filter__sampling_length = 100;

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/
void accel_filter__freertos_task(void *pvParams);
uint16_t accel_filter__get_position(void);

#ifdef __cplusplus
} /* extern "C" */
#endif
