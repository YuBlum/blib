#include <blib.h>
#include <stdio.h>
#include <string.h>

#define test01 STR("test01")

static pixel *buff_test;
static f32 time = 0;

void
__conf(blib_config *config) {
  config->camera_width = 320;
  config->camera_height = 240;
}

void
__init(void) {
  asset_load(ASSET_ATLAS, test01);
  buff_test = texture_buff_create(16, 16, 0);
  for (u32 i = 0; i < 16; i++) {
    for (u32 j = 0; j < 16; j++) {
      buff_test[i * 16 + j].color.a = 0xff;
      buff_test[i * 16 + j].color.r = 0x00;
      buff_test[i * 16 + j].color.g = i * 10;
      buff_test[i * 16 + j].color.b = j * 10;
    }
  }
}

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
  batch->atlas = test01;
  batch->texture_buff = buff_test;

  clear_screen(COL_RED);

  draw_tile(V2U(1, 1), V2F(-40, 20), V2F(1, 1), COL_WHITE, 0);
  draw_text(V2F_0, V2F(1, 1), COL_WHITE, 0, STR("Hello, World!\nhey hey@@"));
  draw_texture_buff(V2F(20, 20), V2F(64, 64), COL_WHITE, 0, 0);
  draw_texture_buff(V2F(20, 20), V2F(64, 64), COL_WHITE, 0, 0);

  submit_batch();
}

void
__quit(void) {
}

