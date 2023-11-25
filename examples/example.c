#include <blib.h>
#include <stdio.h>

void
__conf(void) {
}

void
__init(void) {
}

void
__loop(void) {
}

void
__draw(void) {
  clear_screen(COL_YELLOW);
  draw_quad((v2) { -0.5f, -0.5f }, (v2) { 1.0f, 1.0f }, COL_RED);
  draw_quad((v2) { 0.25f, 0.25f }, (v2) { 0.5f, 0.5f }, COL_BLUE);
}

void
__quit(void) {
}

