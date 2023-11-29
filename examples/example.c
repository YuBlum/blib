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
}

void
__loop(void) {
}

void
__draw(batch *batch) {
  clear_screen(COL_RED);
  batch->atlas = test01;
  draw_quad(V2(-0.5f, -0.5f), V2(1.0f, 1.0f), COL_YELLOW, 0);
  draw_tile(V2U(0, 0), V2(-0.2f, -0.2f), V2(0.5f, 0.5f), COL_WHITE, 0);
  submit_batch();
}

void
__quit(void) {
}

