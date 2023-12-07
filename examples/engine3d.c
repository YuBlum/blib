#include <blib.h>

#define GAME_W 256
#define GAME_H 240
#define GAME_S 2

void
__conf(blib_config *config) {
  config->window_title = "3D Demo";
  config->game_width   = GAME_W;
  config->game_height  = GAME_H;
  config->game_scale   = GAME_S;
}

void
__init(void) {
}

void
__draw(void) {
  clear_screen(COL_MAGENTA);
}

void
__loop(void) {
}

void
__tick(void) {
}

void
__quit(void) {
}
