#ifndef __BLIB_H__
#define __BLIB_H__

#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

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

typedef union {
  u64 u64[2];
  u32 u32[4];
  u16 u16[8];
  u8  u8[16];
} u128;

/*
 * ****************************
 * ****************************
 * *********** MATH ***********
 * ****************************
 * ****************************
 */

/*
 * *** Vectors ***
 * */

typedef struct { f32 x, y;       } v2;
typedef struct { f32 x, y, z;    } v3;
typedef struct { f32 x, y, z, w; } v4;

static inline v2  v2_add(v2 a, v2 b)   { return (v2) { a.x + b.x, a.y + b.y };                       }
static inline v2  v2_sub(v2 a, v2 b)   { return (v2) { a.x - b.x, a.y - b.y };                       }
static inline v2  v2_mul(v2 a, v2 b)   { return (v2) { a.x * b.x, a.y * b.y };                       }
static inline v2  v2_div(v2 a, v2 b)   { return (v2) { a.x / b.x, a.y / b.y };                       }
static inline f32 v2_dot(v2 a, v2 b)   { return a.x * b.x + a.y * b.y;                               }
static inline f32 v2_mag(v2 a)         { return sqrtf(a.x * a.x + a.y * a.y);                        }
static inline f32 v2_dist(v2 a, v2 b)  { return v2_mag(v2_sub(b, a));                                }
static inline v3  v3_add(v3 a, v3 b)   { return (v3) { a.x + b.x, a.y + b.y, a.z + b.z };            }
static inline v3  v3_sub(v3 a, v3 b)   { return (v3) { a.x - b.x, a.y - b.y, a.z - b.z };            }
static inline v3  v3_mul(v3 a, v3 b)   { return (v3) { a.x * b.x, a.y * b.y, a.z * b.z };            }
static inline v3  v3_div(v3 a, v3 b)   { return (v3) { a.x / b.x, a.y / b.y, a.z / b.z };            }
static inline f32 v3_dot(v3 a, v3 b)   { return a.x * b.x + a.y * b.y + a.z * b.z;                   }
static inline f32 v3_mag(v3 a)         { return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);            }
static inline f32 v3_dist(v3 a, v3 b)  { return v3_mag(v3_sub(b, a));                                }
static inline v4  v4_add(v4 a, v4 b)   { return (v4) { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }
static inline v4  v4_sub(v4 a, v4 b)   { return (v4) { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; }
static inline v4  v4_mul(v4 a, v4 b)   { return (v4) { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w }; }
static inline v4  v4_div(v4 a, v4 b)   { return (v4) { a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w }; }
static inline f32 v4_dot(v4 a, v4 b)   { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;       }
static inline f32 v4_mag(v4 a)         { return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);            }
static inline f32 v4_dist(v4 a, v4 b)  { return v4_mag(v4_sub(b, a));                                }

/*
 * *** Matrix ***
 * */

typedef f32 m2x2[2][2];
typedef f32 m2x3[2][3];
typedef f32 m2x4[2][4];

typedef f32 m3x2[3][2];
typedef f32 m3x3[3][3];
typedef f32 m3x4[3][4];

typedef f32 m4x2[4][2];
typedef f32 m4x3[4][3];
typedef f32 m4x4[4][4];

/*
 * ****************************
 * ****************************
 * ******** DATA TYPES ********
 * ****************************
 * ****************************
 */

/*
 * *** String ***
 * */

typedef struct {
  u32  size;
  u32  capa;
  cstr buff;
} str;
#define STR(S) ((str) { sizeof (S) - 1, 0, S })
#define STR_0 ((str) { 0 })

/* Creates a empty dynamic string or copy of `src` depending if `src.size` is 0 or not. */
extern str string_create(str src);

/* Increases the `str` capacity to the specified `amount`.
 * `str` must have been created by `string_create()`. */
void string_reserve(str *str, u32 amount);

/* Copy the `src` string on to the `dest` string.
 * `dest` must have been created by `string_create()`. */
extern void string_copy(str *dest, str src);

/* Concatenate the `src` string with the `dest` string.
 * `dest` must have been created by `string_create()`. */
extern void string_concat(str *dest, str src);

/* Compares if two strings are equal.
 */
extern b8 string_equal(str s1, str s2);

/* Creates a view into the `src` string from `start` to `end`. */
extern str string_view(str src, u32 start, u32 end);

/* Creates a new dynamic string with a subset of `src` from `start` to `end`. */
extern str string_sub(str src, u32 start, u32 end);

/* Insert the `src` string on `dest` string at the specified `index`.
 * `dest` must have been created by `string_create()`. */
extern void string_insert(str *dest, str src, u32 index);

/* Finds the first apperance of the character `c` on `str`. */
extern s8 *string_find_first(str str, s8 c);

/* Finds the last apperance of the character `c` on `str`. */
extern s8 *string_find_last(str str, s8 c);

/* Reverses the contents of a string
 * `str` must have been created by `string_create()`. */
extern void string_reverse(str str);

/* Destroys strings that've been created by `string_create()`. */
extern void string_destroy(str str);

/*
 *
 * *** Array List ***
 *
 * */

/* Creates an array list of `type_size` in bytes then return it. */
extern void *array_list_create(u32 type_size);

/* Get the array list's capacity. */
extern u32   array_list_capacity(void *arr);

/* Get the array list's size. */
extern u32   array_list_size(void *arr);

/* Reserves an `amount` with in a array list.
 * There's a chance of reallocation. */
extern void *array_list_reserve(void *arr, u32 amount);

/* Grows the array list's size by `amount`.
 * There's a chance of reallocation. */
extern void *array_list_grow(void *arr, u32 amount);

/* Shifts the array list to the left starting on `index` by a certain `amount`. */
extern void array_list_shift_left(void *arr, u32 index, u32 amount);

/* Shifts the array list to the right starting on `index` by a certain `amount`. */
extern void array_list_shift_right(void *arr, u32 index, u32 amount);

/* Grows the array list's size and places the value of `item` at the end.
 * There's a chance of reallocation. */
#define array_list_push(ARR, ITEM) do { ARR = array_list_grow(ARR, 1); ARR[array_list_size(ARR) - 1] = ITEM; } while (0)

/* Shrinks the array list's size, removing the last item. Places the deleted value on `out`. */
extern void  array_list_pop(void *arr, void *out);

/* Grows the array list's size and places the value of `item` at the specified `index`.
 * There's a chance of reallocation. */
#define array_list_insert(ARR, INDEX, ITEM) do {\
  if (INDEX <= array_list_size(ARR) - 1) {\
    ARR = array_list_grow(ARR, 1);\
    array_list_shift_right(ARR, INDEX, 1);\
  } else {\
    ARR = array_list_grow(ARR, INDEX - array_list_size(ARR) + 1);\
  }\
  ARR[INDEX] = ITEM;\
  \
} while (0)

/* Shrinks the array list's size, removing the item at `index`. Places the deleted value on `out`. */
extern void  array_list_remove(void *arr, u32 index, void *out);

/* Clears all the content of an array list. */
extern void array_list_clear(void *arr);

/* Destroys the list. */
extern void  array_list_destroy(void *arr);

/*
 *
 * *** Hash Table ***
 *
 * */

typedef struct hash_table hash_table;
typedef enum {
  HT_STR = 0,
  HT_U64,
  HT_U128,
  HT_AMOUNT
} hash_table_type;

/* Creates a hash table. */
extern hash_table *hash_table_create(u32 type_size, hash_table_type key_type);

/* Returns a pointer to the value located at the `key` of a hash table.
 * In case the `key` doesn't exists returns NULL.
 * */
extern void *hash_table_get(hash_table *ht, void *key);

/* Sets `out_key` pointer value to the key of the pointer to a value.
 * `val` is a pointer returned by `hash_table_get` or `hash_table_add`
 * */
extern void hash_table_value_key(hash_table *ht, void *val, void *out_key);

/* Adds a new key on a hash table then returns a pointer to the value. */
extern void *hash_table_add(hash_table *ht, void *key);

/* Deletes an existing key of a hash table. */
extern void hash_table_del(hash_table *ht, void *key);

/* Destroys a hash table. */
extern void hash_table_destroy(hash_table *ht);

/*
 * ****************************
 * ****************************
 * ********** ENGINE **********
 * ****************************
 * ****************************
 */

/*
 * *** Entity System ***
 */

typedef struct {
  u32 type;
  u128 id;
} entity;

/* Begin the creation of a entity type. */
extern void entity_type_begin(str name);

/* Adds a component to the current entity type being created then return the component id. */
extern void entity_type_add_component(str name, u32 size);

/* End the creation of a entity type */
extern void entity_type_end(void);

/* Returns an array list of the specified component of the specific type. */
extern void *entity_type_get_components(str type_name, str comp_name);

/* Creates a new entity of the specified type */
extern entity entity_create(str type_name);

/* Returns a pointer to the component of a specific entity. */
extern void *entity_get_component(entity e, str comp_name);

/* Destroys an entity. */
extern void entity_destroy(entity e);

/*
 * A configuration struct to setup the app
 * */
typedef struct {
  cstr title;
  s32  width;
  s32  height;
  b8   center;
  b8   resizable;
  u32  scenes_amount;
  b8   compatibility_profile;
  u32  opengl_major;
  u32  opengl_minor;
} blib_config;

typedef void (*scene_begin_fn)(void);
typedef void (*scene_update_fn)(void);
typedef void (*scene_end_fn)(void);

typedef struct {
  scene_begin_fn  scene_begin;
  scene_update_fn scene_update;
  scene_end_fn    scene_end;
} scene;

#endif/*__BLIB_H__*/
