//Pacman movement with boundary conditions
main.c 

static void pacman_task(void *params) {
  // left,riht, up , down - direction of pacman, row and column start positions
  // Add (right and left below and above) //width and height 32
  int row_right = matrix_width - 3, row_left = 2, row_up = 5, row_down = matrix_width - 6;
  int col_right = 7, col_left = matrix_height - 8, col_up = 4, col_down = matrix_height - 5;

  // int i = 2, j = matrix_height - 2, k= ,l;
  int count = 0, var;
  direction_e direction = LEFT_UP;
  displayGridBorders(Blue);
  updateDisplay();
  while (true) {
    updateDisplay();
    if (direction == LEFT_UP && count == 5) {
      count = 0;
      // Boundaries choosen based on dimensions of pacman
      switch (row_left) {
      case 2 ... matrix_width - 8:
        drawPackMan(row_left, col_left, direction);
        updateDisplay();
        row_left++;
        break;
      case matrix_width - 7 ... matrix_width: // Check this condition
        var = rand();
        if (var % 2) {
          direction = DOWN_LEFT;
        } else {
          direction = RIGHT_UP;
        }
        row_left = 2;
        break;
      }
    }
    if (direction == RIGHT_UP && count == 5) {
      count = 0;
      switch (row_right) {
      case 7 ... matrix_height - 3:
        drawPackMan(row_right, col_left, direction);
        updateDisplay();
        row_right--;
        break;
      case 0 ... 6: // Check this condition
        var = rand();
        if (var % 2) {
          direction = LEFT_UP;
        } else {
          direction = DOWN_RIGHT;
        }
        row_right = matrix_height - 3;
        break;
      }
    }
    if (direction == DOWN_LEFT && count == 5) {
      count = 0;
      switch (col_down) {
      case 9 ... matrix_height - 5:
        drawPackMan(row_down, col_down, direction);
        updateDisplay();
        col_down--;
        break;
      case 0 ... 8: // Check this condition
        var = rand();
        if (var % 2) {
          direction = RIGHT_DOWN;
        } else {
          direction = UP_LEFT;
        }
        col_down = matrix_height - 5;
        break;
      }
    }
    if (direction == UP_LEFT && count == 5) {
      count = 0;
      switch (col_up) {
      case 4 ... matrix_width - 10:
        drawPackMan(row_down, col_up, direction);
        updateDisplay();
        col_up++;
        break;
      case matrix_width - 9 ... matrix_width: // Check this condition
        var = rand();
        if (var % 2) {
          direction = RIGHT_UP;
        } else {
          direction = DOWN_LEFT;
        }
        col_up = 4;
        break;
      }
    }

    if (direction == RIGHT_DOWN && count == 5) {
      count = 0;
      switch (row_right) {
      case 7 ... matrix_height - 3:
        drawPackMan(row_right, col_right, direction);
        updateDisplay();
        row_right--;
        break;
      case 0 ... 6: // Check this condition
        var = rand();
        if (var % 2) {
          direction = UP_RIGHT;
        } else {
          direction = LEFT_DOWN;
        }
        row_right = matrix_height - 3;
        break;
      }
    }
    if (direction == LEFT_DOWN && count == 5) {
      count = 0;
      switch (row_left) {
      case 2 ... matrix_width - 8:
        drawPackMan(row_left, col_right, direction);
        updateDisplay();
        row_left++;
        break;
      case matrix_width - 7 ... matrix_width: // Check this condition
        var = rand();
        if (var % 2) {
          direction = UP_LEFT;
        } else {
          direction = RIGHT_DOWN;
        }
        row_left = 2;
        break;
      }
    }

    if (direction == UP_RIGHT && count == 5) {
      count = 0;
      switch (col_up) {
      case 4 ... matrix_width - 10:
        drawPackMan(row_up, col_up, direction);
        updateDisplay();
        col_up++;
        break;
      case matrix_width - 9 ... matrix_width: // Check this condition
        var = rand();
        if (var % 2) {
          direction = LEFT_UP;
        } else {
          direction = DOWN_RIGHT;
        }
        col_up = 4;
        break;
      }
    }
    if (direction == DOWN_RIGHT && count == 5) {
      count = 0;
      switch (col_down) {
      case 9 ... matrix_height - 5:
        drawPackMan(row_up, col_down, direction);
        updateDisplay();
        col_down--;
        break;
      case 0 ... 8: // Check this condition
        var = rand();
        if (var % 2) {
          direction = LEFT_DOWN;
        } else {
          direction = UP_RIGHT;
        }
        col_down = matrix_height - 5;
        break;
      }
    }
    count++;
    vTaskDelay(5);
  }
}

graphic.c:
/**************Setting  Lines- Begins*****************/
void set_line1(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right) {
  Color packman_color = Lime;
  if (var_left || var_right) {
    setPixel(row, column, packman_color);
    setPixel(row, column - 1, packman_color);
    setPixel(row, column + 1, packman_color);
  } else if (var_up || var_down) {
    setPixel(row, column, packman_color);
    setPixel(row - 1, column, packman_color);
    setPixel(row + 1, column, packman_color);
  }
}
void set_line2(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right) {
  Color packman_color = Lime;
  if (var_left || var_right) {
    setPixel(var_left ? row + 1 : row - 1, column + 2, packman_color);
    setPixel(var_left ? row + 1 : row - 1, column + 1, packman_color);
    setPixel(var_left ? row + 1 : row - 1, column + 0, packman_color);
    setPixel(var_left ? row + 1 : row - 1, column - 1, packman_color);
    setPixel(var_left ? row + 1 : row - 1, column - 2, packman_color);
  } else if (var_up || var_down) {
    setPixel(row + 2, var_down ? column - 1 : column + 1, packman_color);
    setPixel(row + 1, var_down ? column - 1 : column + 1, packman_color);
    setPixel(row + 0, var_down ? column - 1 : column + 1, packman_color);
    setPixel(row - 1, var_down ? column - 1 : column + 1, packman_color);
    setPixel(row - 2, var_down ? column - 1 : column + 1, packman_color);
  }
}
void set_line3(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right) {
  Color packman_color = Lime;
  if (var_left || var_right) {
    setPixel(var_left ? row + 2 : row - 2, column + 3, packman_color);
    setPixel(var_left ? row + 2 : row - 2, column + 2, packman_color);
    setPixel(var_left ? row + 2 : row - 2, column + 1, packman_color);
    setPixel(var_left ? row + 2 : row - 2, column + 0, packman_color);
    setPixel(var_left ? row + 2 : row - 2, column - 1, packman_color);
    setPixel(var_left ? row + 2 : row - 2, column - 2, packman_color);
    setPixel(var_left ? row + 2 : row - 2, column - 3, packman_color);
  } else if (var_up || var_down) {
    setPixel(row + 3, var_down ? column - 2 : column + 2, packman_color);
    setPixel(row + 2, var_down ? column - 2 : column + 2, packman_color);
    setPixel(row + 1, var_down ? column - 2 : column + 2, packman_color);
    setPixel(row + 0, var_down ? column - 2 : column + 2, packman_color);
    setPixel(row - 1, var_down ? column - 2 : column + 2, packman_color);
    setPixel(row - 2, var_down ? column - 2 : column + 2, packman_color);
    setPixel(row - 3, var_down ? column - 2 : column + 2, packman_color);
  }
}
void set_line4(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right) {
  Color packman_color = Lime;
  if (var_left || var_right) {
    setPixel(var_left ? row + 3 : row - 3, column - 3, packman_color);
    setPixel(var_left ? row + 3 : row - 3, column - 2, packman_color);
    setPixel(var_left ? row + 3 : row - 3, column - 1, packman_color);
    setPixel(var_left ? row + 3 : row - 3, column + 1, packman_color);
    setPixel(var_left ? row + 3 : row - 3, column + 2, packman_color);
    setPixel(var_left ? row + 3 : row - 3, column + 3, packman_color);
  } else if (var_up || var_down) {
    setPixel(row + 3, var_down ? column - 3 : column + 3, packman_color);
    setPixel(row + 2, var_down ? column - 3 : column + 3, packman_color);
    setPixel(row + 1, var_down ? column - 3 : column + 3, packman_color);
    setPixel(row - 1, var_down ? column - 3 : column + 3, packman_color);
    setPixel(row - 2, var_down ? column - 3 : column + 3, packman_color);
    setPixel(row - 3, var_down ? column - 3 : column + 3, packman_color);
  }
}
void set_line5(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right) {
  Color packman_color = Lime;
  if (var_left || var_right) {
    setPixel(var_left ? row + 4 : row - 4, column + 3, packman_color);
    setPixel(var_left ? row + 4 : row - 4, column + 2, packman_color);
    setPixel(var_left ? row + 4 : row - 4, column - 2, packman_color);
    setPixel(var_left ? row + 4 : row - 4, column - 3, packman_color);
  } else if (var_up || var_down) {
    setPixel(row + 3, var_down ? column - 4 : column + 4, packman_color);
    setPixel(row + 2, var_down ? column - 4 : column + 4, packman_color);
    setPixel(row - 2, var_down ? column - 4 : column + 4, packman_color);
    setPixel(row - 3, var_down ? column - 4 : column + 4, packman_color);
  }
}
void set_line6(int row, int column, bool var_up, bool var_down, bool var_left, bool var_right) {
  Color packman_color = Lime;
  if (var_left || var_right) {
    setPixel(var_left ? row + 5 : row - 5, column + 3, packman_color);
    setPixel(var_left ? row + 5 : row - 5, column - 3, packman_color);
  } else if (var_up || var_down) {
    setPixel(row + 3, var_down ? column - 5 : column + 5, packman_color);
    setPixel(row - 3, var_down ? column - 5 : column + 5, packman_color);
  }
}

/**************Setting Lines - Ends*****************/

void drawPackMan(int row, int column, direction_e direction) {
  fillFrameBuffer_Inside_Grid_Black();
  bool var_up = 0, var_down = 0, var_left = 0, var_right = 0;

  if (direction == LEFT_UP || direction == LEFT_DOWN) {
    var_left = 1;
  }
  if (direction == UP_RIGHT || direction == UP_LEFT) {
    var_up = 1;
  }
  if (direction == RIGHT_DOWN || direction == RIGHT_UP) {
    var_right = 1;
  }
  if (direction == DOWN_LEFT || direction == DOWN_RIGHT) {
    var_down = 1;
  }
  set_line1(row, column, var_up, var_down, var_left, var_right);
  set_line2(row, column, var_up, var_down, var_left, var_right);
  set_line3(row, column, var_up, var_down, var_left, var_right);
  set_line4(row, column, var_up, var_down, var_left, var_right);
  set_line5(row, column, var_up, var_down, var_left, var_right);
  set_line6(row, column, var_up, var_down, var_left, var_right);
  // setPixel(row + 6, column, packman_color);
}
