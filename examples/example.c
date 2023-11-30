#include <blib.h>
#include <stdio.h>
#include <string.h>

#define test01 STR("test01")

void
__conf(blib_config *config) {
}

void
__init(void) {
  asset_load(ASSET_ATLAS, test01);
  texture_atlas_setup(test01, 13, 14, 2, 2);
}

void
__loop(void) {
  v2f camera_position = camera_get_position();
  camera_set_position(v2f_add(camera_position, V2F(1, 1)));
}

void
__draw(batch *batch) {
  clear_screen(COL_RED);
  batch->atlas = test01;
  draw_quad(V2F(0, 0), V2F(32, 32), COL_YELLOW, 0);
  draw_tile(V2U(1, 1), V2F(-40, 20), V2F(32, 32), COL_WHITE, 0);
  submit_batch();
}

void
__quit(void) {
}

