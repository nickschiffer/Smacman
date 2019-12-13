

#include "pacman_graphics.h"

void pacman_task_level1(void *task_param) {
  pacman_s pacman = *(pacman_s *)task_param;
  bool blue_pacman_start, green_pacman_start;
  game_graphics_players pac_player;
  int var;
  ball_s ball_pos;

  while (true) {

    if (pacman.packman_color == BLUE) {
      led_matrix__fill_frame_buffer_inside_grid_upper_half(); // Player 1
      pac_player = PLAYER_1;
      ball_pos = get_ball_position_direction();
      if (ball_pos.col == (matrix_width / 2) + 1 && ball_pos.yDir > 0) {
        // printf("here blue1: %d\n", ball_pos.col);
        blue_pacman_start = get_blue_pacman_start();
        if (blue_pacman_start == 1) {
          clear_blue_pacman_start();
          blue_pacman_setup(&pacman);
        }
      }
    } else if (pacman.packman_color == GREEN) {
      led_matrix__fill_frame_buffer_inside_grid_lower_half(); // Player 2
      pac_player = PLAYER_2;
      ball_pos = get_ball_position_direction();
      if (ball_pos.col == (matrix_width / 2) - 1 && ball_pos.yDir < 0) {
        // printf("here green1 : %d\n", ball_pos.col);
        green_pacman_start = get_green_pacman_start();
        if (green_pacman_start == 1) {
          clear_green_pacman_start();
          green_pacman_setup(&pacman);
        }
      }
    }

    // pac_detection_level1(&pacman);

    if (pacman.direction == LEFT_UP) {
      switch (pacman.row_left_upordown) {
      case 2 ... matrix_width - 8:
        game_graphics_packman(pacman.row_left_upordown, pacman.col_leftorrigt_up, pacman.direction,
                              pacman.packman_color, pac_player);
        // led_matrix__update_display();
        pacman.row_left_upordown++;
        break;
      case matrix_width - 7 ... matrix_width: // Check this condition
        var = rand();
        if (var % 2) {
          pacman.direction = DOWN_LEFT;
        } else {
          pacman.direction = RIGHT_UP;
        }
        pacman.row_left_upordown = 2;
        break;
      }
    }
    if (pacman.direction == RIGHT_UP) {
      switch (pacman.row_right_upordown) {
      case 7 ... matrix_width - 3:
        game_graphics_packman(pacman.row_right_upordown, pacman.col_leftorrigt_up, pacman.direction,
                              pacman.packman_color, pac_player);
        // led_matrix__update_display();
        pacman.row_right_upordown--;
        break;
      case 0 ... 6: // Check this condition
        var = rand();
        if (var % 2) {
          pacman.direction = DOWN_RIGHT;
        } else {
          pacman.direction = LEFT_UP;
        }
        pacman.row_right_upordown = matrix_width - 3;
        break;
      }
    }

    // change
    if (pacman.direction == DOWN_LEFT) {
      if (pacman.packman_color == BLUE) {
        switch (pacman.col_down_leftorright) {
        case (matrix_height / 2) + 5 ... matrix_height - 5:
          game_graphics_packman(pacman.row_upordown_left, pacman.col_down_leftorright, pacman.direction,
                                pacman.packman_color, pac_player);
          // led_matrix__update_display();
          pacman.col_down_leftorright--;
          break;
        case 0 ...(matrix_height / 2) + 4: // Check this condition
          var = rand();
          if (var % 2) {
            pacman.direction = RIGHT_DOWN;
          } else {
            pacman.direction = UP_LEFT;
          }
          pacman.col_down_leftorright = matrix_height - 5;
          break;
        }
      } else if (pacman.packman_color == GREEN) {
        switch (pacman.col_down_leftorright) {
        case 9 ...(matrix_height / 2) - 1:
          game_graphics_packman(pacman.row_upordown_left, pacman.col_down_leftorright, pacman.direction,
                                pacman.packman_color, pac_player);
          // led_matrix__update_display();
          pacman.col_down_leftorright--;
          break;
        case 0 ... 8: // Check this condition
          var = rand();
          if (var % 2) {
            pacman.direction = RIGHT_DOWN;
          } else {
            pacman.direction = UP_LEFT;
          }
          pacman.col_down_leftorright = (matrix_height / 2) - 1;
          break;
        }
      }
    }
    // change
    if (pacman.direction == UP_LEFT) {
      if (pacman.packman_color == BLUE) {
        switch (pacman.col_up_leftorright) {
        case (matrix_height / 2)... matrix_width - 10:
          game_graphics_packman(pacman.row_upordown_left, pacman.col_up_leftorright, pacman.direction,
                                pacman.packman_color, pac_player);
          // led_matrix__update_display();
          pacman.col_up_leftorright++;
          break;
        case matrix_height - 9 ... matrix_height:
          var = rand();
          if (var % 2) {
            pacman.direction = RIGHT_UP;
          } else {
            pacman.direction = DOWN_LEFT;
          }
          pacman.col_up_leftorright = (matrix_height / 2);
          break;
        }
      } else if (pacman.packman_color == GREEN) {
        switch (pacman.col_up_leftorright) {
        case 4 ...(matrix_height / 2) - 6:
          game_graphics_packman(pacman.row_upordown_left, pacman.col_up_leftorright, pacman.direction,
                                pacman.packman_color, pac_player);
          // led_matrix__update_display();
          pacman.col_up_leftorright++;
          break;
        case (matrix_height / 2) - 5 ... matrix_height:
          var = rand();
          if (var % 2) {
            pacman.direction = RIGHT_UP;
          } else {
            pacman.direction = DOWN_LEFT;
          }
          pacman.col_up_leftorright = 4;
          break;
        }
      }
    }

    if (pacman.direction == RIGHT_DOWN) {
      switch (pacman.row_right_upordown) {
      case 7 ... matrix_height - 3:
        game_graphics_packman(pacman.row_right_upordown, pacman.col_leftorrigt_down, pacman.direction,
                              pacman.packman_color, pac_player);
        // led_matrix__update_display();
        pacman.row_right_upordown--;
        break;
      case 0 ... 6:
        var = rand();
        if (var % 2) {
          pacman.direction = UP_RIGHT;
        } else {
          pacman.direction = LEFT_DOWN;
        }
        pacman.row_right_upordown = matrix_height - 3;
        break;
      }
    }
    if (pacman.direction == LEFT_DOWN) {
      switch (pacman.row_left_upordown) {
      case 2 ... matrix_width - 8:
        game_graphics_packman(pacman.row_left_upordown, pacman.col_leftorrigt_down, pacman.direction,
                              pacman.packman_color, pac_player);
        // led_matrix__update_display();
        pacman.row_left_upordown++;
        break;
      case matrix_width - 7 ... matrix_width:
        var = rand();
        if (var % 2) {
          pacman.direction = UP_LEFT;
        } else {
          pacman.direction = RIGHT_DOWN;
        }
        pacman.row_left_upordown = 2;
        break;
      }
    }
    // change
    if (pacman.direction == UP_RIGHT) {
      if (pacman.packman_color == BLUE) {
        switch (pacman.col_up_leftorright) {
        case (matrix_height / 2)... matrix_height - 10:
          game_graphics_packman(pacman.row_upordown_right, pacman.col_up_leftorright, pacman.direction,
                                pacman.packman_color, pac_player);
          // led_matrix__update_display();
          pacman.col_up_leftorright++;
          break;
        case matrix_height - 9 ... matrix_height:
          var = rand();
          if (var % 2) {
            pacman.direction = LEFT_UP;
          } else {
            pacman.direction = DOWN_RIGHT;
          }
          pacman.col_up_leftorright = (matrix_height / 2) + 4;
          break;
        }
      } else if (pacman.packman_color == GREEN) {
        switch (pacman.col_up_leftorright) {
        case 4 ...(matrix_height / 2) - 6:
          game_graphics_packman(pacman.row_upordown_right, pacman.col_up_leftorright, pacman.direction,
                                pacman.packman_color, pac_player);
          // led_matrix__update_display();
          pacman.col_up_leftorright++;
          break;
        case (matrix_height / 2) - 5 ... matrix_height:
          var = rand();
          if (var % 2) {
            pacman.direction = LEFT_UP;
          } else {
            pacman.direction = DOWN_RIGHT;
          }
          pacman.col_up_leftorright = 4;
          break;
        }
      }
    }
    // change
    if (pacman.direction == DOWN_RIGHT) {
      if (pacman.packman_color == BLUE) {
        switch (pacman.col_down_leftorright) {
        case (matrix_height / 2) + 5 ... matrix_height - 5:
          game_graphics_packman(pacman.row_upordown_right, pacman.col_down_leftorright, pacman.direction,
                                pacman.packman_color, pac_player);
          // led_matrix__update_display();
          pacman.col_down_leftorright--;
          break;
        case 0 ...(matrix_height / 2) + 4:
          var = rand();
          if (var % 2) {
            pacman.direction = LEFT_DOWN;
          } else {
            pacman.direction = UP_RIGHT;
          }
          pacman.col_down_leftorright = matrix_height - 5;
          break;
        }
      } else if (pacman.packman_color == GREEN) {
        switch (pacman.col_down_leftorright) {
        case 9 ...(matrix_height / 2) - 1:
          game_graphics_packman(pacman.row_upordown_right, pacman.col_down_leftorright, pacman.direction,
                                pacman.packman_color, pac_player);
          // led_matrix__update_display();
          pacman.col_down_leftorright--;
          break;
        case 0 ... 8:
          var = rand();
          if (var % 2) {
            pacman.direction = LEFT_DOWN;
          } else {
            pacman.direction = UP_RIGHT;
          }
          pacman.col_down_leftorright = (matrix_height / 2) - 1;
          break;
        }
      }
    }

    vTaskDelay(50);
  }
}

/// level2
void pacman_task_level2(void *task_param) {
    pacman_s pacman = *(pacman_s *)task_param;
    
    bool blue_pacman_start, green_pacman_start;
    game_graphics_players pac_player;
    ball_s ball_pos;
    // starboard=> right side of the board, port=> leftside of the port
    int8_t ball_starboard = 0, ball_port = 0;
    bool initial_display = 1;
    // bool start_detection;
    while (true) {
        // Task for blue
        if (pacman.packman_color == BLUE) {
            pac_player = PLAYER_1;
            blue_pacman_start = get_blue_pacman_start();
            if (blue_pacman_start == 1) {
                clear_blue_pacman_start();
                pacman.row_left_upordown = 2;
                pacman.row_right_upordown = matrix_width - 3;
                pacman.direction = RIGHT_UP;
            }
            // Which half is the ball located in
            ball_pos = get_ball_position_direction();
            
            if (ball_pos.col == ((matrix_width / 2) + 2) && ball_pos.yDir > 0) {
                ball_starboard = ball_pos.xDir < 0 ? 1 : 0;
                ball_port = ball_pos.xDir > 0 ? 1 : 0;
            }
        }
        // Task for green
        else if (pacman.packman_color == GREEN) {
            pac_player = PLAYER_2;
            green_pacman_start = get_green_pacman_start();
            if (green_pacman_start == 1) {
                clear_green_pacman_start();
                pacman.row_left_upordown = 2;
                pacman.row_right_upordown = matrix_width - 3;
                pacman.direction = LEFT_UP;
            }
            ball_pos = get_ball_position_direction();
            
            if (ball_pos.col == ((matrix_width / 2) - 2) && ball_pos.yDir < 0) {
                // printf("%i\n", ball_pos.xDir);
                ball_starboard = ball_pos.xDir < 0 ? 1 : 0;
                ball_port = ball_pos.xDir > 0 ? 1 : 0;
            }
        }
        
        // start_detection = get_start_detection();
        
        // If pacman is moving in left direction
        if (pacman.direction == LEFT_UP && (ball_starboard == 1 || ball_port == 1 || initial_display == 1)) {
            initial_display = 0;
            if (pacman.packman_color == BLUE) {
                led_matrix__fill_frame_buffer_inside_grid_upper_half(); // Player 1
            } else {
                led_matrix__fill_frame_buffer_inside_grid_lower_half(); // Player 2
            }
            switch (pacman.row_left_upordown) {
                case 2 ... matrix_width - 8:
                    game_graphics_packman(pacman.row_left_upordown, pacman.col_leftorrigt_up, pacman.direction,
                                          pacman.packman_color, pac_player);
                    // led_matrix__update_display();
                    pacman.row_left_upordown =
                    (ball_starboard == 1) ? (pacman.row_left_upordown + 1) : (pacman.row_left_upordown + 2);
                    break;
                case matrix_width - 7 ... matrix_width: // Check this condition
                    ball_starboard = 0;
                    ball_port = 0;
                    initial_display = 1;
                    pacman.direction = RIGHT_UP;
                    pacman.row_left_upordown = 2;
                    break;
            }
        }
        // If pacman is moving in right direction
        if (pacman.direction == RIGHT_UP && (ball_port == 1 || ball_starboard == 1 || initial_display == 1)) {
            initial_display = 0;
            if (pacman.packman_color == BLUE) {
                led_matrix__fill_frame_buffer_inside_grid_upper_half(); // Player 1
            } else {
                led_matrix__fill_frame_buffer_inside_grid_lower_half(); // Player 2
            }
            switch (pacman.row_right_upordown) {
                case 7 ... matrix_width - 3:
                    game_graphics_packman(pacman.row_right_upordown, pacman.col_leftorrigt_up, pacman.direction,
                                          pacman.packman_color, pac_player);
                    // led_matrix__update_display();
                    pacman.row_right_upordown =
                    (ball_port == 1) ? (pacman.row_right_upordown - 1) : (pacman.row_right_upordown - 2);
                    break;
                case 0 ... 6: // Check this condition
                    ball_starboard = 0;
                    ball_port = 0;
                    initial_display = 1;
                    pacman.direction = LEFT_UP;
                    pacman.row_right_upordown = matrix_width - 3;
                    break;
            }
        }
        
        vTaskDelay(100);
    }
}
/// level3
void pacman_task_level3(void *task_param) {
  pacman_s pacman = *(pacman_s *)task_param;

  bool blue_pacman_start, green_pacman_start;
  game_graphics_players pac_player;
  ball_s ball_pos;
  // starboard=> right side of the board, port=> leftside of the port
  int8_t ball_starboard = 0, ball_port = 0;
  bool initial_display = 1;
  bool start_detection;

  while (true) {
    // Task for blue
    if (pacman.packman_color == BLUE) {
      pac_player = PLAYER_1;
      blue_pacman_start = get_blue_pacman_start();
      if (blue_pacman_start == 1) {
        clear_blue_pacman_start();
        pacman.row_left_upordown = 2;
        pacman.row_right_upordown = matrix_width - 3;
        pacman.direction = LEFT_UP;
      }
      // Which half is the ball located in
      ball_pos = get_ball_position_direction();

      if (ball_pos.col > ((matrix_width / 2) + 1) && ball_pos.yDir > 0) {
        ball_starboard = ball_pos.xDir < 0 ? 1 : 0;
        ball_port = ball_pos.xDir > 0 ? 1 : 0;
      } else {
        ball_starboard = 0;
        ball_port = 0;
      }
    }
    // Task for green
    else if (pacman.packman_color == GREEN) {
      pac_player = PLAYER_2;
      green_pacman_start = get_green_pacman_start();
      if (green_pacman_start == 1) {
        clear_green_pacman_start();
        pacman.row_left_upordown = 2;
        pacman.row_right_upordown = matrix_width - 3;
        pacman.direction = RIGHT_UP;
      }
      ball_pos = get_ball_position_direction();
      if (ball_pos.col < ((matrix_width / 2) - 1) && ball_pos.yDir < 0) {
        // printf("%i\n", ball_pos.xDir);
        ball_starboard = ball_pos.xDir < 0 ? 1 : 0;
        ball_port = ball_pos.xDir > 0 ? 1 : 0;
      } else {
        ball_starboard = 0;
        ball_port = 0;
      }
    }

    start_detection = get_start_detection();

    // If pacman is moving in left direction
    if (pacman.direction == LEFT_UP && start_detection == 1 &&
        (ball_starboard == 1 || ball_port == 1 || initial_display == 1)) {
      initial_display = 0;
      if (pacman.packman_color == BLUE) {
        led_matrix__fill_frame_buffer_inside_grid_upper_half(); // Player 1
      } else {
        led_matrix__fill_frame_buffer_inside_grid_lower_half(); // Player 2
      }
      if (pacman.row_left_upordown > ball_pos.row) {
        pacman.direction = RIGHT_UP;
        pacman.row_right_upordown = pacman.row_left_upordown;
        //   pacman.col_leftorrigt_down = pacman.col_leftorrigt_up;
      }
      if (pacman.col_leftorrigt_up > ball_pos.col) {
        // pacman.direction = RIGHT_UP;
        pacman.col_leftorrigt_up--;
        //   pacman.col_leftorrigt_down = pacman.col_leftorrigt_up;
      } else {
        pacman.col_leftorrigt_up++;
      }
      switch (pacman.row_left_upordown) {
      case 2 ... matrix_width - 8:
        game_graphics_packman(pacman.row_left_upordown, pacman.col_leftorrigt_up, pacman.direction,
                              pacman.packman_color, pac_player);
        if (ball_starboard == 1) {
          pacman.row_left_upordown = pacman.row_left_upordown + 1;
        }
        if (ball_port == 1) {
          pacman.row_left_upordown = pacman.row_left_upordown + 2;
        }
        break;
      case matrix_width - 7 ... matrix_width: // Check this condition
        ball_starboard = 0;
        ball_port = 0;
        initial_display = 1;
        pacman.direction = RIGHT_UP;
        pacman.row_left_upordown = 2;
        break;
      }
    }

    //  If pacman is moving in right direction
    if (pacman.direction == RIGHT_UP && start_detection == 1 &&
        (ball_port == 1 || ball_starboard == 1 || initial_display == 1)) {
      initial_display = 0;
      if (pacman.packman_color == BLUE) {
        led_matrix__fill_frame_buffer_inside_grid_upper_half(); // Player 1
      } else {
        led_matrix__fill_frame_buffer_inside_grid_lower_half(); // Player 2
      }
      if (pacman.row_right_upordown < ball_pos.row) {
        pacman.direction = LEFT_UP;
        pacman.row_left_upordown = pacman.row_right_upordown;
        //   pacman.col_leftorrigt_up = pacman.col_leftorrigt_down;
      }
      if (pacman.col_leftorrigt_up > ball_pos.col) {
        // pacman.direction = RIGHT_UP;
        pacman.col_leftorrigt_up--;
        //   pacman.col_leftorrigt_down = pacman.col_leftorrigt_up;
      } else {
        pacman.col_leftorrigt_up++;
      }
      switch (pacman.row_right_upordown) {
      case 7 ... matrix_width - 3:
        game_graphics_packman(pacman.row_right_upordown, pacman.col_leftorrigt_up, pacman.direction,
                              pacman.packman_color, pac_player);
        if (ball_starboard == 1) {
          pacman.row_right_upordown = pacman.row_right_upordown - 2;
        }
        if (ball_port == 1) {
          pacman.row_right_upordown = pacman.row_right_upordown - 1;
        }
        break;
      case 0 ... 6: // Check this condition
        ball_starboard = 0;
        ball_port = 0;
        initial_display = 1;
        pacman.direction = LEFT_UP;
        pacman.row_right_upordown = matrix_width - 3;
        break;
      }
    }

    if (ball_starboard == 0 && ball_port == 0) {
      if (pacman.packman_color == BLUE) {
        led_matrix__fill_frame_buffer_inside_grid_upper_half(); // Player 1
        game_graphics_packman(pacman.row_left_upordown, pacman.col_leftorrigt_up, pacman.direction,
                              pacman.packman_color, pac_player);
      } else {
        led_matrix__fill_frame_buffer_inside_grid_lower_half(); // Player 2
        game_graphics_packman(pacman.row_right_upordown, pacman.col_leftorrigt_up, pacman.direction,
                              pacman.packman_color, pac_player);
      }
    }

    vTaskDelay(180);
  }
}
