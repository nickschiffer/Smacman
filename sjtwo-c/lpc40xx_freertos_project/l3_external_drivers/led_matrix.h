#ifndef LED_MATRIX_H
#define LED_MATRIX_H
#include "FreeRTOS.h"
#include "delay.h"
#include "gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * r1,g1,b1 --> for first  32 rows
 * r2,g2,b2 --> for second 32 rows
 * a,b,c,d,e --> each row select - 2^5 = 32
 * oe --> inverted set low to enable display
 * latch --> set high to enable latch
 * clk
 **/
static gpio_s r1, g1, b1, r2, g2, b2, A, B, C, D, E, oe, latch, clk;

typedef enum { // RGB (000 => Off)
  Off,
  Blue,
  Green,
  Cyan,
  Red,
  Pink,
  Lime,
  White
} Color;

typedef enum { RedPlane, GreenPlane, BluePlane } ColorPlane;

uint64_t farmeBuffer[64][3];

void led_matrix_init();
void enable_display();
void disable_display();
void enable_latch();
void disable_latch();
void selectRow(int row);
void clearPixel(int row, int column);
void setPixel(int row, int col, Color color);
void setRowData(int row, Color color, uint64_t data);
void setRowDataRaw(int row, ColorPlane plane, uint64_t data);
void clearFrameBuffer();
void fillFrameBuffer(uint64_t data, Color color);
void updateDisplay();

#endif