#include "blib.h"
#include <stdio.h>

b8
window_init(u32 width, u32 height, b8 resizable, cstr name) {
  printf("window = {\n");
  printf("  width     = %u,\n", width);
  printf("  height    = %u,\n", height);
  printf("  resizable = %s,\n", resizable ? "true" : "false");
  printf("  name      = \"%s\",\n", name);
  printf("};\n");
  return 0;
}
