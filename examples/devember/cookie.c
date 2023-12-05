#include <blib.h>
#include <math.h>
#include <stdio.h>

#define GAME_W 96
#define GAME_H 96
#define GAME_S 6

f32 game_time;

void
__conf(blib_config *config) {
  config->window_title  = "COOKIE CLICKER";
  config->window_width  = GAME_W * GAME_S;
  config->window_height = GAME_H * GAME_S;
  config->camera_width  = GAME_W;
  config->camera_height = GAME_H;
}

void
__init(void) {
  asset_load(ASSET_ATLAS, STR("cookie"));
  texture_atlas_setup(STR("cookie"), 32, 32, 0, 0);

  game_time = 0.0f;
}

void
__loop(f32 dt) {
  game_time += dt;
}

void
__tick(void) {
}

void
__draw(batch *batch) {
  clear_screen(COL_GREEN);
  batch->atlas = STR("cookie");
  printf("ang: %f\n", sin(game_time) * PI);
  draw_tile(V2U_0, V2F_0, V2F(1, 1), sin(game_time) * PI, COL_WHITE, 0);
  submit_batch();
}

void
__quit(void) {
}
