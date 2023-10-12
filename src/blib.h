#ifndef __BLIB_H__
#define __BLIB_H__

#include <stdint.h>

/*
 *
 * *** Basic Types ***
 *
 * */

typedef  int8_t     b8;
typedef  int8_t     s8;
typedef uint8_t     u8;
typedef  int16_t   s16;
typedef uint16_t   u16;
typedef  int32_t   s32;
typedef uint32_t   u32;
typedef  int64_t   s64;
typedef uint64_t   u64;
typedef float      f32;
typedef double     f64;
typedef char       *cstr;
typedef const char *ccstr;

#define true  ((b8)1)
#define false ((b8)0)



/*
 *
 * *** Window and Context things ***
 *
 * */

/* Sets the OpenGL version to `major`.`minor`.
 * If `compatibility_profile` profile is true then uses the Compatibility profile,
 * else it uses the Core profile */
extern void set_opengl_version(u32 major, u32 minor, b8 compatibility_profile);

/* Creates window with the specified `width`, `height` and `title`.
 * `resizable` defines if the window can be resized.
 * `center` defines if the window spawns on the center of the monitor. */
extern b8 window_init(s32 width, s32 height, cstr title, b8 resizable, b8 center);

/* Return if the window is still running */
extern b8 window_is_running(void);



/*
 *
 * *** Array List ***
 *
 * */

/* Creates an Array List of `type_size` in bytes then return it. */
extern void *array_list_create(u32 type_size);

/* Get the Array List's capacity. */
extern u32   array_list_capacity(void *arr);

/* Get the Array List's size. */
extern u32   array_list_size(void *arr);

/* Grows the Array List's size by `amount`.
 * There's a chance of reallocation. */
extern void *array_list_grow(void *arr, u32 amount);

/* Shifts the Array List to the left starting on `index` by a certain `amount`. */
extern void array_list_shift_left(void *arr, u32 index, u32 amount);

/* Shifts the Array List to the right starting on `index` by a certain `amount`. */
extern void array_list_shift_right(void *arr, u32 index, u32 amount);

/* Grows the Array List's size and places the value of `item` at the end.
 * There's a chance of reallocation. */
#define array_list_push(ARR, ITEM) do { array_list_grow(ARR, 1); ARR[array_list_size(ARR) - 1] = ITEM; } while (0)

/* Shrinks the Array List's size, removing the last item. Places the deleted value on `out`. */
extern void  array_list_pop(void *arr, void *out);

/* Grows the Array List's size and places the value of `item` at the specified `index`.
 * There's a chance of reallocation. */
extern void *array_list_insert(void *arr, u32 index, void *item);
#define array_list_insert(ARR, INDEX, ITEM) do {\
  array_list_grow(ARR, 1); array_list_shift_right(ARR, INDEX, 1); ARR[INDEX] = ITEM; } while (0)

/* Shrinks the Array List's size, removing the item at `index`. Places the deleted value on `out`. */
extern void  array_list_remove(void *arr, u32 index, void *out);

/* Destroys the list. */
extern void  array_list_destroy(void *arr);

#endif/*__BLIB_H__*/
