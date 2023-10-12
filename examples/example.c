#include <stdio.h>
#include <blib.h>

s32
main(void) {
  s32 *numbers = array_list_create(sizeof (s32));

  array_list_push(numbers, 2);
  array_list_push(numbers, 9);
  array_list_push(numbers, 8);
  array_list_push(numbers, 6);
  array_list_push(numbers, 6);
  array_list_push(numbers, 3);

  for (u32 i = 0; i < array_list_size(numbers); i++) {
    printf("numbers[%u] = %u;\n", i, numbers[i]);
  }
  printf("-----------------\n");

  array_list_pop(numbers, 0);
  array_list_pop(numbers, 0);

  for (u32 i = 0; i < array_list_size(numbers); i++) {
    printf("numbers[%u] = %u;\n", i, numbers[i]);
  }
  printf("-----------------\n");

  array_list_insert(numbers, 0, 0);
  array_list_insert(numbers, 2, 1);
  array_list_insert(numbers, 7, 3);
  array_list_insert(numbers, 5, 2);

  for (u32 i = 0; i < array_list_size(numbers); i++) {
    printf("numbers[%u] = %u;\n", i, numbers[i]);
  }
  printf("-----------------\n");

  array_list_remove(numbers, 3, 0);
  array_list_remove(numbers, 0, 0);

  for (u32 i = 0; i < array_list_size(numbers); i++) {
    printf("numbers[%u] = %u;\n", i, numbers[i]);
  }
  printf("-----------------\n");

  printf("numbers.size = %u;\n", array_list_size(numbers));
  printf("numbers.capa = %u;\n", array_list_capacity(numbers));

  array_list_destroy(numbers);


#if 0
  window_init(640, 480, "example", false, true);
  while (window_is_running()) {
  }
#endif
  return 0;
}
