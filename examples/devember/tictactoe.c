#include <blib.h>
#include <stdio.h>

#define GAME_W 80
#define GAME_H 80
#define GAME_S 8

#define BOARD_SIZE 3

#define TILE_SIZE 16

typedef enum {
  TILE_EMPTY = ' ',
  TILE_X     = 'X',
  TILE_O     = 'O'
} tile_state;

typedef struct {
  tile_state state;
  v2f pos;
  b8 win;
  u32 win_tile_x;
} board_tile;

static board_tile board[BOARD_SIZE][BOARD_SIZE];
static v2u selected_tile;
static tile_state current_player;
static b8 someone_win;

void
__conf(blib_config *blib_config) {
  blib_config->window_title = "TIC TAC TOE";
  blib_config->game_width  = GAME_W;
  blib_config->game_height = GAME_H;
  blib_config->game_scale  = GAME_S;
}

static void
game_start(void) {
  for (u32 i = 0; i < BOARD_SIZE; i++) {
    for (u32 j = 0; j < BOARD_SIZE; j++) {
      board[i][j].pos.x = -GAME_W * 0.5f + TILE_SIZE * (1 + i) + TILE_SIZE * 0.5f;
      board[i][j].pos.y = -GAME_W * 0.5f + TILE_SIZE * (1 + j) + TILE_SIZE * 0.5f;
      board[i][j].state = TILE_EMPTY;
      board[i][j].win   = false;
    }
  }

  selected_tile.x = 3;
  selected_tile.y = 3;

  current_player = 'X';
  someone_win    = false;
}

void
__init(void) {
  asset_load(ASSET_ATLAS, STR("tictactoe"));
  game_start();
}

void
__loop(f32 dt) {
  if (key_press('Q')) close_window();

  if (key_press(' ')) game_start();

  if (someone_win) return;

  b8 founded_mouse_tile = false;
  for (u32 i = 1; i <= BOARD_SIZE; i++) {
    for (u32 j = 1; j <= BOARD_SIZE; j++) {
      v2f mouse_pos = mouse_get_position();
      v2u mouse_tile = {
        (mouse_pos.x + GAME_W * 0.5f) / TILE_SIZE,
        (mouse_pos.y + GAME_H * 0.5f) / TILE_SIZE
      };
      if (j == mouse_tile.x && i == mouse_tile.y) {
        selected_tile = v2u_sub_scalar(mouse_tile, 1);
        founded_mouse_tile = true;
        break;
      }
    }
  }
  if (!founded_mouse_tile) {
    selected_tile.x = 3;
    selected_tile.y = 3;
  }

  if (board[selected_tile.x][selected_tile.y].state == ' ' &&
      button_click(BTN_LEFT) && selected_tile.x < 3 && selected_tile.y < 3) {
    board[selected_tile.x][selected_tile.y].state = current_player;
    current_player = current_player == 'X' ? 'O' : 'X';
  }

  /* check for row wins */
  for (u32 i = 0; i < 3; i++) {
    if (board[0][i].state == ' ') continue;
    if (board[0][i].state == board[1][i].state &&
        board[0][i].state == board[2][i].state) {
      for (u32 j = 0; j < 3; j++) {
        board[j][i].win = true;
        board[j][i].win_tile_x = 0;
        someone_win = true;
      }
    }
  }

  /* check for column wins */
  for (u32 i = 0; i < 3; i++) {
    if (board[i][0].state == ' ') continue;
    if (board[i][0].state == board[i][1].state &&
        board[i][0].state == board[i][2].state) {
      for (u32 j = 0; j < 3; j++) {
        board[i][j].win = true;
        board[i][j].win_tile_x = 1;
        someone_win = true;
      }
    }
  }

  /* check for middle left wins */
  if (board[1][1].state != ' ') {
    if (board[1][1].state == board[2][0].state &&
        board[1][1].state == board[0][2].state) {
      board[2][0].win = true;
      board[2][0].win_tile_x = 2;
      someone_win = true;

      board[0][2].win = true;
      board[0][2].win_tile_x = 2;
      someone_win = true;

      board[1][1].win = true;
      board[1][1].win_tile_x = 2;
      someone_win = true;
    }
  }

  /* check for middle right wins */
  if (board[1][1].state != ' ') {
    if (board[1][1].state == board[0][0].state &&
        board[1][1].state == board[2][2].state) {
      for (u32 i = 0; i < 3; i++) {
        board[i][i].win = true;
        board[i][i].win_tile_x = 3;
        someone_win = true;
      }
    }
  }

}

void
__tick(void) {
}

void
__draw(batch *batch) {
  batch->atlas = STR("tictactoe");
  clear_screen(COL_BLACK);
  for (u32 i = 0; i < BOARD_SIZE; i++) {
    for (u32 j = 0; j < BOARD_SIZE; j++) {
      printf("selected_tile = { %u, %u }\n"
             "current_tile  = { %u, %u }\n\n", selected_tile.x, selected_tile.y, j, i);
      if (i < BOARD_SIZE - 1) {
        draw_tile(V2U(1, 1), board[i][j].pos, V2F(1, 1), 0, COL_WHITE, 0);
      }
      if (j > 0) {
        draw_tile(V2U(0, 1), board[i][j].pos, V2F(1, 1), 0, COL_WHITE, 0);
      }
      switch (board[i][j].state) {
        case 'X':
            draw_tile(V2U(0, 0), board[i][j].pos, V2F(1, 1), 0, COL_WHITE, 0);
          break;
        case 'O':
            draw_tile(V2U(1, 0), board[i][j].pos, V2F(1, 1), 0, COL_WHITE, 0);
          break;
        case ' ':
          if ((selected_tile.x == i && selected_tile.y == j) && !someone_win) {
            draw_tile(V2U(current_player == 'O', 0), board[i][j].pos, V2F(1, 1), 0, COL_WHITE, 0);
          }
          break;
      }
      if (board[i][j].win) {
        draw_tile(V2U(board[i][j].win_tile_x, 2), board[i][j].pos, V2F(1, 1), 0, COL_WHITE, 0);
      }
    }
  }
  submit_batch();
}

void
__quit(void) {
}
