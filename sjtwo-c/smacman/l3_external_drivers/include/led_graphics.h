#pragma once

#include "led_matrix.h"
void led_matrix__displayGridBorders(led_matrix__color_e color);
void led_matrix__drawPaddle_blue(int row, int column, led_matrix__direction_e direction);
void led_matrix__drawPaddle_green(int row, int column, led_matrix__direction_e direction);
void led_matrix__drawPackMan(int row, int column, led_matrix__direction_e direction, led_matrix__color_e packman_color);
void led_matrix__drawBall(int row, int column, led_matrix__color_e ball_color);

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
