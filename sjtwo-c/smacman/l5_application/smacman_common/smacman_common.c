#include "smacman_common.h"
#include "ball.h"
#include "game_logic.h"

QueueHandle_t blue_paddle_direction_queue;
QueueHandle_t green_paddle_direction_queue;

void common__splash_screen() {

  led_matrix__display_intro_GridBorder();
  led_matrix__display_intro_Pacs();
}

void controller_poll_ready_or_pause_and_take_action(QueueHandle_t *state_queue) {
  static uint8_t player1_counter = 0, player2_counter = 0;
  bool current_switch_player_1, current_switch_player_2;
  game_logic_game_state_s game_current_set_state;
  static bool player1__ready = false, player2__ready = false, player_1__pause_flag = false,
              player_2__pause_flag = false, is_paused = false, all_ready = false;
  current_switch_player_1 = controller_com__get_player_1_button();

  if (current_switch_player_1 == true) {
    printf("Switch Status  = %i, player_1 = %i\n", current_switch_player_1, player1_counter);

    if ((player1_counter % 2) == 0) {
      player1__ready = true;
    } else {
      printf("Pausing\n");
      player_1__pause_flag = true;
    }
    player1_counter++;
  }
  current_switch_player_2 = controller_com__get_player_2_button();
  if (current_switch_player_2 == true) {
    if ((player2_counter % 2) == 0) {
      player2__ready = true;
    } else {
      player_2__pause_flag = true;
    }
    player2_counter++;
  }

  if (player1__ready == true && player2__ready == true) {
    game_current_set_state = IN_PROGRESS_STATE;
    xQueueSend(*(state_queue), &game_current_set_state, portMAX_DELAY);
    player1__ready = false;
    player2__ready = false;
    all_ready = true;
  } else if (player_1__pause_flag == true || player_2__pause_flag == true) {
    printf("sending Pausing\n");

    game_current_set_state = IN_PAUSE_STATE;
    xQueueSend(*(state_queue), &game_current_set_state, portMAX_DELAY);
    player_1__pause_flag = false;
    player_2__pause_flag = false;
    is_paused = true;
  }
  if (all_ready == true && is_paused == true && (player1__ready == true || player2__ready == true)) {
    game_current_set_state = IN_PROGRESS_STATE;
    xQueueSend(*(state_queue), &game_current_set_state, portMAX_DELAY);
    player1__ready = false;
    player2__ready = false;
    is_paused = false;
  }
}