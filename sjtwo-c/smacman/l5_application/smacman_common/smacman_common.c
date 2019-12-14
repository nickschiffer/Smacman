#include "smacman_common.h"
#include "ball.h"
#include "game_logic.h"

void common__splash_screen() {

  led_matrix__display_intro_GridBorder();
  led_matrix__display_intro_Pacs();
}

void controller_poll_ready_or_pause_and_take_action(QueueHandle_t *state_queue) {
  uint8_t player1_counter = 0, player2_counter = 0;
  bool current_switch_player_1, current_switch_player_2;
  game_logic_game_state_s game_current_set_state;
  current_switch_player_1 = controller_com__get_player_1_button();

  if (current_switch_player_1 == true) {
    if ((player1_counter % 2) == 0) {
      game_current_set_state = IN_PROGRESS_STATE;
      xQueueSend(*(state_queue), &game_current_set_state, portMAX_DELAY);
    } else {
      game_current_set_state = IN_PAUSE_STATE;
      xQueueSend(*(state_queue), &game_current_set_state, portMAX_DELAY);
    }
    player1_counter++;
  }
  current_switch_player_2 = controller_com__get_player_2_button();
  if (current_switch_player_2 == true) {
    if ((player2_counter % 2) == 0) {
      game_current_set_state = IN_PROGRESS_STATE;
      xQueueSend(*(state_queue), &game_current_set_state, portMAX_DELAY);
    } else {
      game_current_set_state = IN_PAUSE_STATE;
      xQueueSend(*(state_queue), &game_current_set_state, portMAX_DELAY);
    }
    player1_counter++;
  }
}