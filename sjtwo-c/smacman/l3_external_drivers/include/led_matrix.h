#pragma once
#include "FreeRTOS.h"
#include "delay.h"
#include "gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define matrix_height 64UL
#define matrix_width 64UL
#define row_bound 32UL
#define data_size uint64_t

static const data_size ALL_INSIDE_GRID = 0x0FFFFFFFFFFFFFF0;
static const data_size LOWER_INSIDE_GRID = 0x00000000FFFFFFF0;
static const data_size UPPER_INSIDE_GRID = 0x0FFFFFFF00000000;

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
  OFF,
  BLUE,
  GREEN,
  CYAN,
  RED,
  PINK,
  LIME,
  WHITE
} led_matrix__color_e;

typedef enum { RED_PLANE, GREEN_PLANE, BLUE_PLANE } led_matrix__color_plane_e;

typedef enum {
  LEFT_UP,
  LEFT_DOWN,
  RIGHT_UP,
  RIGHT_DOWN,
  UP_RIGHT,
  UP_LEFT,
  DOWN_RIGHT,
  DOWN_LEFT
} led_matrix__direction_e;

data_size frame_buffer[64][3];

void led_matrix__init();
void led_matrix__enable_display();
void led_matrix__disable_display();
void led_matrix__enable_latch();
void led_matrix__disable_latch();
void led_matrix__select_row(int row);
void led_matrix__clear_pixel(int row, int column);
bool led_matrix__get_pixel(int row, int column);
void led_matrix__set_pixel(int row, int col, led_matrix__color_e color);
void led_matrix__set_row_data(int row, led_matrix__color_e color, uint64_t data);
void led_matrix__set_row_data_raw(int row, led_matrix__color_plane_e plane, uint64_t data);
void led_matrix__clear_frame_buffer();
void led_matrix__fill_frame_buffer(uint64_t data, led_matrix__color_e color);
void led_matrix__update_display();

void led_matrix_clear_frame_buffer_inside_grid(data_size data);
