#include <blib.h>
#include <stdio.h>
#include <string.h>

#define test01 STR("test01")

v2f pos;
#define SPEED 100

f32 ang = 0;

v2f p;

void
__conf(blib_config *config) {
  config->game_width  = 320;
  config->game_height = 240;
  config->game_scale  = 2;
}

void
__init(void) {
  asset_load(ASSET_ATLAS, test01);
}

void
__loop(f32 dt) {
  if (key_click('Q')) {
    close_window();
  }

  v2f vel = V2F(key_press('D') - key_press('A'), key_press('W') - key_press('S'));
  if (vel.x != 0 && vel.y != 0) {
    vel = v2f_unit(vel);
  }
  pos.x += vel.x * SPEED * dt;
  pos.y += vel.y * SPEED * dt;


  ang += dt;


  p = mouse_get_position();
}

void
__tick(void) {
}

void
__draw(batch *batch) {
  batch->atlas = test01;

  clear_screen(COL_RED);

  draw_tile(V2U(0, 0), pos, V2F(1, 1), V2F_0, 0, COL_YELLOW, 0);

  draw_line(pos, p, 1, COL_WHITE, 1);

  draw_text(V2F(-20, -20), V2F(1, 1), COL_WHITE, 0, STR("Angle: %.2f"), ang);

  draw_triangle(V2F(-50, -50), V2F(0, 50), V2F(50, -50), COL_YELLOW, 0);

  submit_batch();
}

void
__quit(void) {
}

