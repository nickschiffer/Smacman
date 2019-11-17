#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#define matrix_height 64
#define matrix_width 64
#define column_bound 64
#define row_bound 32
#define data_size uint64_t

#include "FreeRTOS.h"
#include "delay.h"
#include "gpio.h"
#include <stdbool.h>
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

// Direction-side
typedef enum { LEFT_UP, LEFT_DOWN, RIGHT_UP, RIGHT_DOWN, UP_RIGHT, UP_LEFT, DOWN_RIGHT, DOWN_LEFT } direction_e;

typedef enum { RedPlane, GreenPlane, BluePlane } ColorPlane;

data_size farmeBuffer[column_bound][3];

void led_matrix_init();
void enable_display();
void disable_display();
void enable_latch();
void disable_latch();
void selectRow(int row);
void clearPixel(int row, int column);
void setPixel(int row, int col, Color color);
void setRowData(uint32_t row, Color color, data_size data);
void setRowDataRaw(int row, ColorPlane plane, data_size data);
void clearFrameBuffer();
void fillFrameBuffer(data_size data, Color color);
void fillFrameBuffer_Inside_Grid_Black();
void updateDisplay();
void set_line1_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right);
void set_line2_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right);
void set_line3_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right);
void set_line4_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right);
void set_line5_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right);
void set_line6_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right);
void set_line1_ball(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right);
void set_line2_ball(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right);
void set_line3_ball(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right);
void set_line4_ball(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right);
void set_line5_ball(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right);

#endif
