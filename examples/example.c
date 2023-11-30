#include <blib.h>
#include <stdio.h>
#include <string.h>

#define test01 STR("test01")

void
__conf(blib_config *config) {
  config->camera_width = 320;
  config->camera_height = 240;
}

void
__init(void) {
  asset_load(ASSET_ATLAS, test01);
}

static f32 time = 0;

void
__loop(f32 dt) {
  time += dt;
  f32 speed = 100.0f;

  v2f camera_position = camera_get_position();
  camera_set_position(
    v2f_add(
      camera_position,
      V2F(
        speed * dt * (key_press('D') - key_press('A')),
        speed * dt * (key_press('W') - key_press('S'))
      )
    )
  );

  if (button_click(BTN_LEFT)) {
    close_window();
  }
}

void
__draw(batch *batch) {
  clear_screen(COL_RED);
  batch->atlas = test01;
  draw_quad(V2F(0, 0), V2F(32, 32), COL_YELLOW, 0);
  draw_tile(V2U(1, 1), V2F(-40, 20), V2F(16, 16), COL_WHITE, 0);
  submit_batch();
}

void
__quit(void) {
}

