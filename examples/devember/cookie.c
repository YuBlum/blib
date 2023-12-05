#include <blib.h>
#include <math.h>
#include <stdio.h>

#define GAME_W 96
#define GAME_H 96
#define GAME_S 6

f32 game_time;

f32 cookie_scale;

b8 clicked_cookie;

u32 clicked_amount;

#define COOKIE_POS V2F_0

#define COOKIE_ROTATION_SPEED 0.1f

void
__conf(blib_config *config) {
  config->window_title  = "COOKIE CLICKER";
  config->game_width  = GAME_W;
  config->game_height = GAME_H;
  config->game_scale  = GAME_S;
}

void
__init(void) {
  asset_load(ASSET_ATLAS, STR("cookie"));
  texture_atlas_setup(STR("cookie"), 32, 32, 0, 0);

  game_time = 0.0f;
  cookie_scale = 1.0f;
  clicked_cookie = false;

  clicked_amount = 0;
}

void
__loop(f32 dt) {
  game_time += dt;

  if (!clicked_cookie) {
    if (v2f_dist(mouse_get_position(), COOKIE_POS) < 16) {
      cookie_scale = 1.05f;
      if (button_press(BTN_LEFT)) { 
        cookie_scale = 0.95f;
        clicked_cookie = true;
      }
    } else {
      cookie_scale = 1.0f;
    }
  } else if (!button_press(BTN_LEFT)) {
    clicked_amount++;
    clicked_cookie = false;
  }
}

void
__tick(void) {
}

void
__draw(batch *batch) {
  clear_screen(V4F(48.0f / 256.0f, 96.0f / 256.0f, 130.0f / 256.0f, 256.0f / 256.0f));
  batch->atlas = STR("cookie");
  draw_tile(V2U_0, COOKIE_POS, V2F(cookie_scale, cookie_scale),
      COOKIE_ROTATION_SPEED * game_time, COL_WHITE, 0);
  draw_text(
      V2F(-GAME_W * 0.5f + 4, GAME_H * 0.5f - 4),
      V2F(0.5f, 0.5f),
      COL_WHITE,
      0,
      STR("CLICKED %u TIMES"),
      clicked_amount);
  submit_batch();
}

void
__quit(void) {
}
