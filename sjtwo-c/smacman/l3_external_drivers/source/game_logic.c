#include "game_logic.h"

game_logic_game_state_s game_state_value = 0;

game_logic_game_state_s get_game_state(void) { return game_state_value; }
void set_game_state(game_logic_game_state_s game_state) { game_state_value = game_state; }