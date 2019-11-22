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

data_size frame_buffer[matrix_height][3];

// Direction-side
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

void led_matrix__init();
void led_matrix__enable_display();
void led_matrix__disable_display();
void led_matrix__enable_latch();
void led_matrix__disable_latch();
void led_matrix__select_row(int row);
void led_matrix__clear_pixel(int row, int column);
bool led_matrix__get_pixel(int row, int column);
void led_matrix__set_pixel(int row, int col, led_matrix__color_e color);
void led_matrix__set_row_data(int row, led_matrix__color_e color, data_size data);
void led_matrix__set_row_data_raw(int row, led_matrix__color_plane_e plane, data_size data);
void led_matrix__clear_frame_buffer();
void led_matrix__fill_frame_buffer_inside_grid();
void led_matrix__fill_frame_buffer_inside_grid_lower_half();
void led_matrix__fill_frame_buffer_inside_grid_upper_half();
void led_matrix__fill_frame_buffer(data_size data, led_matrix__color_e color);
void led_matrix__update_display();
void led_matrix__set_line1_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right,
                               led_matrix__color_e packman_color);
void led_matrix__set_line2_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right,
                               led_matrix__color_e packman_color);
void led_matrix__set_line3_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right,
                               led_matrix__color_e packman_color);
void led_matrix__set_line4_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right,
                               led_matrix__color_e packman_color);
void led_matrix__set_line5_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right,
                               led_matrix__color_e packman_color);
void led_matrix__set_line6_pac(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right,
                               led_matrix__color_e packman_color);
void led_matrix__set_line1_ball(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right);
void led_matrix__set_line2_ball(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right);
void led_matrix__set_line3_ball(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right);
void led_matrix__set_line4_ball(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right);
void led_matrix__set_line5_ball(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right);
