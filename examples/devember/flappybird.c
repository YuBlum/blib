#include <blib.h>
#include <stdio.h>
#include <stdlib.h>

#define GAME_W 192
#define GAME_H 320
#define GAME_S 2

#define GRAV_SPEED 0.4
#define MAX_GRAV 10

#define JUMP_HEIGHT 5

#define PLAYER_ANIM_SPEED 15

#define PLAYER_ROT_SPEED 150

#define BG_OFFSET_SPEED 50

static v2f player;
static f32 player_vel;
static u32 player_frame;
static f32 player_angle;
static b8  player_died;

static f32 game_time;

static b8 game_started;

static f32 bg_offset;

static v2f *pipes;

static f32 spawn_pipe_timer;

#define GROUND_Y (-GAME_H * 0.5f - 8 + 16)

static u32 score;
static b8 bird_is_on_pipe;
static b8 old_bird_is_on_pipe;

void
__conf(blib_config *config) {
  config->window_title = "FLAPPY BIRD";
  config->game_width   = GAME_W;
  config->game_height  = GAME_H;
  config->game_scale   = GAME_S;
}

static void
reset(void) {
  player = V2F(-32, -16);
  game_started = false;
  player_died = false;
  spawn_pipe_timer = 1;
  score = 0;
  bird_is_on_pipe = false;
  old_bird_is_on_pipe = false;
  array_list_clear(pipes);
}

void
__init(void) {
  asset_load(ASSET_ATLAS, STR("flappybird"));
  game_time = 0;
  pipes = array_list_create(sizeof (v2f));

  reset();
}

void
__loop(f32 dt) {
  if (!game_started && button_click(BTN_LEFT)) {
    reset();
    game_started = true;
  }

  game_time += dt;

  if (!player_died) {
    bg_offset += dt * BG_OFFSET_SPEED;
    if (bg_offset >= 64) {
      bg_offset = 0;
    }
    player_frame = ((u32)(game_time * PLAYER_ANIM_SPEED)) % 4;
  }

  if (!game_started) return;

  if (!player_died) {
    spawn_pipe_timer += 0.5f * dt;
    if (spawn_pipe_timer > 1.0f) {
      array_list_push(pipes, V2F(GAME_W * 0.5f + 16, rand() % (GAME_H - 80) - GAME_H * 0.5f + 32));
      spawn_pipe_timer = 0;
    }

    old_bird_is_on_pipe = bird_is_on_pipe;
    for (u32 i = array_list_size(pipes) - 1; i < (u32)-1; i--) {
      pipes[i].x -= dt * 80;
      bird_is_on_pipe = player.x + 6 > pipes[i].x - 16 && player.x - 6 < pipes[i].x + 16;
      if (pipes[i].x + 16 < -GAME_W * 0.5f) {
        array_list_remove(pipes, i, 0);
      } else if (player.x + 6 > pipes[i].x - 16 &&
                 player.x - 6 < pipes[i].x + 16 &&
                 (player.y + 6 > pipes[i].y + 32 ||
                  player.y - 6 < pipes[i].y - 32)){
        player_died = true;
        game_started = false;
      }
    }
    if (!bird_is_on_pipe && old_bird_is_on_pipe) {
      old_bird_is_on_pipe = false;
      score++;
    }
  }

  if (button_click(BTN_LEFT)) player_angle = 45;

  player_angle -= PLAYER_ROT_SPEED * dt;
  if (player_angle < -90) {
    player_angle = -90;
  }

  if (player.y < GROUND_Y + 16) {
    player_died = true;
    game_started = false;
  }
}

void
__tick(void) {
  if (!game_started) return;

  player_vel += GRAV_SPEED;
  if (player_vel > MAX_GRAV) player_vel = MAX_GRAV;

  if (button_click_tick(BTN_LEFT)) {
    player_vel = -JUMP_HEIGHT;
  }

  player.y -= player_vel;
}

void
__draw(batch *batch) {
  batch->atlas = STR("flappybird");
  clear_screen(V4F(95.0f / 255.0f, 205.0f / 255.0f, 228.0f / 255.0f, 1.0f));

  for (u32 i = 0; i < GAME_W / 64 + 2; i++) {
    f32 x = -GAME_W * 0.5f + 8 + i * 64 - 64 - bg_offset;
    for (u32 j = 0; j < 4; j++) {
      draw_tile(
          V2U(j, 1),
          V2F(x + j * 16, GROUND_Y + 16),
          V2F(1, 1),
          V2F_0, 0, COL_WHITE, 0);
    }
  }

  for (u32 i = 0; i < array_list_size(pipes); i++) {
    f32 top_y = pipes[i].y + 32;
    draw_tile(V2U(4, 1), V2F(pipes[i].x - 8, top_y), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
    draw_tile(V2U(5, 1), V2F(pipes[i].x + 8, top_y), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);

    for (u32 j = 0; j < (GAME_H * 0.5f - top_y) / 16; j++) {
      draw_tile(V2U(4, 0), V2F(pipes[i].x - 8, top_y + j * 16 + 16), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
      draw_tile(V2U(5, 0), V2F(pipes[i].x + 8, top_y + j * 16 + 16), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
    }

    f32 bot_y = pipes[i].y - 32;
    draw_tile(V2U(4, 1), V2F(pipes[i].x - 8, bot_y), V2F(1, -1), V2F_0, 0, COL_WHITE, 0);
    draw_tile(V2U(5, 1), V2F(pipes[i].x + 8, bot_y), V2F(1, -1), V2F_0, 0, COL_WHITE, 0);
    for (u32 j = 0; j < (-(-GAME_H * 0.5f - bot_y)) / 16 - 1; j++) {
      draw_tile(V2U(4, 0), V2F(pipes[i].x - 8, bot_y - j * 16 - 16), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
      draw_tile(V2U(5, 0), V2F(pipes[i].x + 8, bot_y - j * 16 - 16), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
    }
  }

  for (u32 i = 0; i < GAME_W / 16; i++) {
    draw_tile(
        V2U(0, 2),
        V2F(-GAME_W * 0.5f + 8 + i * 16, GROUND_Y),
        V2F(1, 1),
        V2F_0, 0, COL_WHITE, 0);
  }

  draw_tile(V2U(player_frame, 0), player, V2F(1, 1), V2F_0, DEG2RAD(player_angle), COL_WHITE, 0);
  draw_text(V2F(3, GAME_H * 0.5f - 16 - 3), V2F(3, 3), COL_BLACK, 0, STR("%u"), score);
  draw_text(V2F(0, GAME_H * 0.5f - 16), V2F(3, 3), COL_YELLOW, 0, STR("%u"), score);
  submit_batch();
}

void
__quit(void) {
}
