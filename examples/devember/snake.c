#include <blib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_SIZE 640
#define TILE_AMOUNT 10
#define TILE_SIZE   V2F((WINDOW_SIZE / TILE_AMOUNT), (WINDOW_SIZE / TILE_AMOUNT))
#define TILE_GAP    8

static v2f snake_pos;

static v2f *snake_body;
static u32 snake_body_size;

#define MOVE_TIMER_SPEED 5
static f32 move_timer;

typedef enum {
  DIR_TOP = 0,
  DIR_LEFT,
  DIR_RIGHT,
  DIR_BOTTOM
} direction;

static direction snake_dir;

static v2f apple;

static b8 game_over;

static v2f *free_tiles;

static void
apple_new_position(void) {
  apple = free_tiles[(rand() % array_list_size(free_tiles))];
}

void
__conf(blib_config *config) {
  config->window_title  = "SNAKE";
  config->window_width  = WINDOW_SIZE;
  config->window_height = WINDOW_SIZE;
  config->camera_width  = WINDOW_SIZE;
  config->camera_height = WINDOW_SIZE;
}

void
__init(void) {
  srand(time(0));
  snake_pos = V2F(-TILE_SIZE.x * 0.5f, -TILE_SIZE.y * 0.5f);
  move_timer = 0;
  game_over  = true;
  snake_body = array_list_create(sizeof (v2f));
  snake_body = array_list_reserve(snake_body, snake_body_size);
  free_tiles = array_list_create(sizeof (v2f));
  free_tiles = array_list_reserve(free_tiles, TILE_AMOUNT * TILE_AMOUNT);
}

void
start_game(void) {
  snake_pos = V2F(-TILE_SIZE.x * 0.5f, -TILE_SIZE.y * 0.5f);
  move_timer = 0;
  snake_dir = DIR_RIGHT;
  snake_body_size = 1;
  array_list_clear(snake_body);
  game_over  = false;
  array_list_clear(free_tiles);
  for (u32 i = 0; i < TILE_AMOUNT; i++) {
    for (u32 j = 0; j < TILE_AMOUNT; j++) {
      v2f tile_pos = {
        j * TILE_SIZE.x + (TILE_SIZE.x * 0.5f) - (WINDOW_SIZE * 0.5f),
        i * TILE_SIZE.y + (TILE_SIZE.y * 0.5f) - (WINDOW_SIZE * 0.5f)
      };
      if (tile_pos.x == snake_pos.x && tile_pos.y == snake_pos.y) continue;
      array_list_push(free_tiles, tile_pos);
    }
  }
  apple_new_position();
}

void
__loop(f32 dt) {
  if (key_press('Q')) close_window();

  if (game_over) {
    if (key_press(' ')) {
      start_game();
    }
    return;
  }

  if (key_press('W')) snake_dir = DIR_TOP;
  if (key_press('A')) snake_dir = DIR_LEFT;
  if (key_press('D')) snake_dir = DIR_RIGHT;
  if (key_press('S')) snake_dir = DIR_BOTTOM;

  if (move_timer < 1) {
    move_timer += MOVE_TIMER_SPEED * dt;
  } else {
    move_timer = 0;
    array_list_push(snake_body, snake_pos);
    if (array_list_size(snake_body) > snake_body_size) {
      v2f tile_pos;
      array_list_remove(snake_body, 0, &tile_pos);
      array_list_push(free_tiles, tile_pos);
    }
    switch (snake_dir) {
      case DIR_TOP:
        snake_pos.y += TILE_SIZE.y;
        if (snake_pos.y > WINDOW_SIZE * 0.5f)
          snake_pos.y = -WINDOW_SIZE * 0.5f + TILE_SIZE.y * 0.5f;
        break;
      case DIR_LEFT:
        snake_pos.x -= TILE_SIZE.x;
        if (snake_pos.x < -WINDOW_SIZE * 0.5f)
          snake_pos.x = WINDOW_SIZE * 0.5f - TILE_SIZE.x * 0.5f;
        break;
      case DIR_RIGHT:
        snake_pos.x += TILE_SIZE.x;
        if (snake_pos.x > WINDOW_SIZE * 0.5f)
          snake_pos.x = -WINDOW_SIZE * 0.5f + TILE_SIZE.x * 0.5f;
        break;
      case DIR_BOTTOM:
        snake_pos.y -= TILE_SIZE.y;
        if (snake_pos.y < -WINDOW_SIZE * 0.5f)
          snake_pos.y = WINDOW_SIZE * 0.5f - TILE_SIZE.y * 0.5f;
        break;
    }
    for (u32 i = 0; i < array_list_size(free_tiles); i++) {
      if (snake_pos.x == free_tiles[i].x && snake_pos.y == free_tiles[i].y) {
        array_list_remove(free_tiles, i, 0);
        break;
      }
    }
    if (snake_pos.x == apple.x && snake_pos.y == apple.y) {
      apple_new_position();
      snake_body_size++;
    }
    for (u32 i = 0; i < array_list_size(snake_body); i++) {
      if (snake_pos.x == snake_body[i].x && snake_pos.y == snake_body[i].y) {
        game_over = true;
        break;
      }
    }
  }
}

void
__draw(batch *batch) {
  clear_screen(COL_BLACK);
  draw_quad(snake_pos, v2f_sub_scalar(TILE_SIZE, TILE_GAP), game_over ? COL_GRAY : COL_GREEN, 0);
  for (u32 i = 0; i < array_list_size(snake_body); i++) {
    draw_quad(snake_body[i], v2f_sub_scalar(TILE_SIZE, TILE_GAP), game_over ? COL_GRAY : COL_GREEN, 0);
  }
  if (!game_over) {
    draw_quad(apple, v2f_sub_scalar(TILE_SIZE, TILE_GAP), COL_RED, 0);
  }
  submit_batch();
}

void
__quit(void) {
}

