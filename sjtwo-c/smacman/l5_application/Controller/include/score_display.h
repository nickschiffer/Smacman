

/**
 * @file
 * This module controls the 7-seg LED matrix on
 * the Smacman controllers. It is used to display the
 * user's score.
 */
#pragma once
// clang-format off
#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"

#include "i2c.h"
#include "gpio.h"

/*******************************************************************************
 *
 *                               D E F I N E S
 *
 ******************************************************************************/
static const i2c_e        SCORE_DISPLAY__I2C_DEVICE     = I2C__2; 
static const uint8_t      SCORE_DISPLAY__I2C_ADDRESS    = 0xE0;
static const gpio__port_e SCORE_DISPLAY__SDA_GPIO_PORT  = GPIO__PORT_0;
static const gpio__port_e SCORE_DISPLAY__SCL_GPIO_PORT  = GPIO__PORT_0;
static const uint8_t      SCORE_DISPLAY__SDA_GPIO_PIN   = 10;
static const uint8_t      SCORE_DISPLAY__SCL_GPIO_PIN   = 11;

static const uint8_t HT16K33_BLINK_CMD       = 0x80;
static const uint8_t HT16K33_BLINK_DISPLAYON = 0x01;
static const uint8_t HT16K33_CMD_BRIGHTNESS  = 0xE0;
static const uint8_t HT16K33_CMD_OSCILLATOR  = 0x20;
static const uint8_t HT16K33_ON              = 0x01;
static const uint8_t HT16K33_MAX_BRIGHTNESS  =   15;
static const uint8_t HT16K33_MIN_BRIGHTNESS  =    0;

// digits are right to left
static const uint8_t HT16K33_DIGIT0    = 0x00;
static const uint8_t HT16K33_DIGIT1    = 0x02;
static const uint8_t HT16K33_SEMICOLON = 0x04;
static const uint8_t HT16K33_DIGIT2    = 0x06;
static const uint8_t HT16K33_DIGIT3    = 0x08; 
/*******************************************************************************
 *
 *                                 E N U M S
 *
 ******************************************************************************/
typedef enum {
    HT16K33_BLINK_OFF,   
    HT16K33_BLINK_2HZ,   
    HT16K33_BLINK_1HZ,   
    HT16K33_BLINK_HALFHZ
} score_display__blink_rate_e;
/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/
bool score_display__init();
bool score_display__write_score(uint16_t score);
bool score_display__set_brightness(uint8_t brightness);
bool score_display__set_blink_rate(score_display__blink_rate_e blink_rate);

#ifdef __cplusplus
} /* extern "C" */
#endif
// clang-format on
