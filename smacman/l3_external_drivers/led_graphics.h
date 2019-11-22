#pragma once

#include "led_matrix.h"
void led_matrix__displayGridBorders(led_matrix__color_e color);
void led_matrix__drawPaddle_blue(int row, int column, led_matrix__direction_e direction);
void led_matrix__drawPaddle_green(int row, int column, led_matrix__direction_e direction);
void led_matrix__drawPackMan(int row, int column, led_matrix__direction_e direction, led_matrix__color_e packman_color);
void led_matrix__drawBall(int row, int column);