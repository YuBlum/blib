#include <blib.h>
#include <stdio.h>
#include <stdlib.h>

#define CELL_SIZE   8
#define CELL_SIZE_V V2F(CELL_SIZE, CELL_SIZE)

#define BOARD_SIZE 12
#define MINE_AMOUNT 10

#define GAME_W (BOARD_SIZE * CELL_SIZE)
#define GAME_H (GAME_W)
#define GAME_S 6

#define GAME_TOP    (+GAME_H * 0.5f) 
#define GAME_RIGHT  (+GAME_W * 0.5f) 
#define GAME_BOTTOM (-GAME_H * 0.5f) 
#define GAME_LEFT   (-GAME_W * 0.5f) 

typedef struct {
  u32 mines_around;
  b8  placed_flag;
  b8  is_mine;
  b8  is_open;
} cell;

static cell board[BOARD_SIZE][BOARD_SIZE];
static b8 game_started;

void
__conf(blib_config *config) {
  config->window_title = "MINESWEEPER";
  config->game_width   = GAME_W;
  config->game_height  = GAME_H;
  config->game_scale   = GAME_S;
}

void
game_cleanup(void) {
  for (u32 i = 0; i < BOARD_SIZE; i++) {
    for (u32 j = 0; j < BOARD_SIZE; j++) {
      board[j][i].mines_around = 0;
      board[j][i].placed_flag  = false;
      board[j][i].is_open      = false;
      board[j][i].is_mine      = false;
    }
  }

  game_started = false;
}

void
__init(void) {
  asset_load(ASSET_ATLAS, STR("minesweeper"));
  texture_atlas_setup(STR("minesweeper"), CELL_SIZE, CELL_SIZE, 0, 0);
  game_cleanup();
}

void
generate_mines(s32 cant_be_x, s32 cant_be_y) {
  for (u32 i = 0; i < MINE_AMOUNT; i++) {
    s32 x, y;
    do { 
      x = rand() % BOARD_SIZE;
      y = rand() % BOARD_SIZE;
    } while (x == cant_be_x && y == cant_be_y);
    board[x][y].is_mine = true;
  }
  for (u32 i = 0; i < BOARD_SIZE; i++) {
    for (u32 j = 0; j < BOARD_SIZE; j++) {
      if (board[j][i].is_mine) continue;
      b8 has_bottom = i > 0;
      b8 has_left   = j > 0;
      b8 has_top    = i < BOARD_SIZE - 1;
      b8 has_right  = j < BOARD_SIZE - 1;
      if (has_bottom) {
        if (board[j][i - 1].is_mine) {
          board[j][i].mines_around++;
        }
        if (has_left) {
          if (board[j - 1][i - 1].is_mine) {
            board[j][i].mines_around++;
          }
        }
        if (has_right) {
          if (board[j + 1][i - 1].is_mine) {
            board[j][i].mines_around++;
          }
        }
      }
      if (has_top) {
        if (board[j][i + 1].is_mine) {
          board[j][i].mines_around++;
        }
        if (has_left) {
          if (board[j - 1][i + 1].is_mine) {
            board[j][i].mines_around++;
          }
        }
        if (has_right) {
          if (board[j + 1][i + 1].is_mine) {
            board[j][i].mines_around++;
          }
        }
      }
      if (has_left) {
        if (board[j - 1][i].is_mine)
          board[j][i].mines_around++;
      }
      if (has_right) {
        if (board[j + 1][i].is_mine)
          board[j][i].mines_around++;
      }
    }
  }
}

b8
open_cell(s32 x, s32 y, b8 open_mine) {
  if (x < 0 || x > BOARD_SIZE - 1 || y < 0 || y > BOARD_SIZE - 1 ||
      board[x][y].is_open) return false;
  if (board[x][y].is_mine) {
    if (open_mine) board[x][y].is_open = true;
    return open_mine;
  }
  board[x][y].is_open = true;
  if (board[x][y].mines_around == 0) {
    open_cell(x - 1, y - 1, false);
    open_cell(x - 1, y + 0, false);
    open_cell(x - 1, y + 1, false);

    open_cell(x + 0, y - 1, false);
    open_cell(x + 0, y + 1, false);

    open_cell(x + 1, y - 1, false);
    open_cell(x + 1, y + 0, false);
    open_cell(x + 1, y + 1, false);
  }
  return false;
}

void
__loop(f32 dt) {
  for (s32 i = 0; i < BOARD_SIZE; i++) {
    for (s32 j = 0; j < BOARD_SIZE; j++) {
      v2f mouse_pos = mouse_get_position();
      v2i mouse_cell = {
        (mouse_pos.x + GAME_W * 0.5f) / CELL_SIZE,
        (mouse_pos.y + GAME_H * 0.5f) / CELL_SIZE
      };
      if (j == mouse_cell.x && i == mouse_cell.y) {
        if (button_click(BTN_RIGHT) && !board[j][i].is_open) {
          board[j][i].placed_flag = !board[j][i].placed_flag;
        }
        else if (button_click(BTN_LEFT) && !board[j][i].placed_flag) {
          if (!game_started) {
            game_started = true;
            generate_mines(j, i);
          }
          if (open_cell(j, i, true)) {
            for (s32 k = 0; k < BOARD_SIZE; k++) {
              for (s32 l = 0; l < BOARD_SIZE; l++) {
                board[l][k].is_open = true;
              }
            }
          }
          break;
        }
      }
    }
  }
}

void
__tick(void) {
}

void
__draw(batch *batch) {
  batch->atlas = STR("minesweeper");
  clear_screen(COL_GRAY);
  for (u32 i = 0; i < BOARD_SIZE; i++) {
    for (u32 j = 0; j < BOARD_SIZE; j++) {
      v2f pos = {
        .x = GAME_LEFT   + j * CELL_SIZE + CELL_SIZE * 0.5f,
        .y = GAME_BOTTOM + i * CELL_SIZE + CELL_SIZE * 0.5f
      };
      if (board[j][i].is_open) {
        if (board[j][i].is_mine) {
          draw_tile(V2U(3, 0), pos, V2F(1, 1), 0, COL_WHITE, 0);
        } else {
          draw_tile(V2U(2, 0), pos, V2F(1, 1), 0, COL_WHITE, 0);
          if (board[j][i].mines_around > 0) {
            v4f color;
            switch (board[j][i].mines_around) {
              case 1: color = COL_BLUE;       break;
              case 2: color = COL_GREEN;      break;
              case 3: color = COL_RED;        break;
              case 4: color = COL_YELLOW;     break;
              case 5: color = COL_BROWN;      break;
              case 6: color = COL_PURPLE;     break;
              case 7: color = COL_GRAY;       break;
              case 8: color = COL_BLACK;      break;
            }
            draw_text(pos, V2F(1, 1), color, 0, STR("%u"), board[j][i].mines_around);
          }
        }
      } else {
        draw_tile(V2U(board[j][i].placed_flag, 0), pos, V2F(1, 1), 0, COL_WHITE, 0);
      }
    }
  }
  submit_batch();
}

void
__quit(void) {
}
