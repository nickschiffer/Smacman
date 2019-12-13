#pragma once
#include "ball.h"

void increase_ball_x(ball_s *ball);
void decrease_ball_x(ball_s *ball);
void ball_setup(ball_s *ball);
// getter setter for blue pacman
void set_blue_pacman_start(void);
void clear_blue_pacman_start(void);
bool get_blue_pacman_start(void);
// getter setter for green pacman
void set_green_pacman_start(void);
void clear_green_pacman_start(void);
bool get_green_pacman_start(void);

// Update ball information at the boundary
void set_ball_position_direction(ball_s *ball);
ball_s get_ball_position_direction(void);

// Setter and Getter for ball half location
void set_ball_in_green_half(bool in_green_half);
void set_ball_in_blue_half(bool in_blue_half);
bool isBallInGreenHalf();
bool isBallinBlueHalf();

// Setter and Getter for starboard/port side
