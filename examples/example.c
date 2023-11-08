#include <stdio.h>
#include <blib.h>

s32
main(void) {
  str dyn_str = string_create(STR("hello, ."));
  printf("%.*s\n", dyn_str.size, dyn_str.buff);
  string_insert(&dyn_str, STR("YESS"), 7);
  printf("%.*s\n", dyn_str.size, dyn_str.buff);
  string_concat(&dyn_str, STR("world!"));
  printf("%.*s\n", dyn_str.size, dyn_str.buff);
  string_reverse(dyn_str);
  printf("%.*s\n", dyn_str.size, dyn_str.buff);

  str test  = STR("A[[[B[C]D]]]E");
  s8 *last  = string_find_last(test, ']');
  s8 *first = string_find_first(test, '[');

  printf("%lu %lu\n", (u64)last - (u64)test.buff, (u64)first - (u64)test.buff);

  str viewst = string_view(test, 4, 8);
  printf("%.*s\n", viewst.size, viewst.buff);

  str subst = string_sub(test, 1, 3);
  printf("%.*s\n", subst.size, subst.buff);

  string_destroy(dyn_str);
  string_destroy(subst);

  return 0;
  window_init(640, 480, STR("example"), false, true);
  while (window_is_running()) {
  }
}
