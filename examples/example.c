#include <blib.h>
#include <stdio.h>
#include <string.h>

#define GAME_W 256
#define GAME_H 240
#define GAME_S 4


void
__conf(blib_config *config) {
}

void
__init(void) {
}

void
__loop(void) {
}

void
__draw(str *batch_shaders) {
  clear_screen(COL_RED);
  draw_quad((v2) {-0.5f, -0.5f}, (v2) { 0.5f, 0.5f }, COL_YELLOW, 0);
  submit_batch();
}

void
__quit(void) {
}

