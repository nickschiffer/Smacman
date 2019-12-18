#pragma once
#include "ball.h"

typedef enum { INIT_STATE = 0, IN_PROGRESS_STATE = 1, IN_SCORE_STATE = 2, IN_PAUSE_STATE = 3 } game_logic_game_state_s;

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

// Setter and Getter for blue/green player
void set_players_score(uint8_t score_green, uint8_t score_blue);

uint8_t get_blue_player_score(void);

uint8_t get_green_player_score(void);

uint8_t max_get_max_score(void);