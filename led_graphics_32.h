#ifndef LED_GRAPHICS_H
#define LED_GRAPHICS_H

#include "led_matrix_32.h"

void drawPackMan(int row, int column, direction_e turn_left);
void drawPaddle1(int row, int column, direction_e direction);
void drawPaddle2(int row, int column, direction_e direction);
void drawBall(int row, int column, direction_e direction);

#endif
