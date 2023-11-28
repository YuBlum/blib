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
 * *** Helpers ***
 */

#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))

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
 * *** Matrices ***
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

/* Clears all hash table. */
extern void hash_table_clear(hash_table *ht);

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

/* A game entity struct. */
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

/* All the entities will be destroyed. */
extern void entity_type_clear(str name);

/* Creates a new entity of the specified type and puts into `e` */
extern void entity_create(str type_name, entity *e);

/* Returns a pointer to the component of a specific entity. */
extern void *entity_get_component(entity *e, str comp_name);

/* Destroys an entity. */
extern void entity_destroy(entity *e);

/*
 * *** Shader ***
 * */

typedef s32 uniform;

/* Gets an uniform location of a shader */
extern uniform shader_get_uniform(str shader_name, str uniform_name);

/* Sets an integer `uniform` of the current setted shader to `value`. */
extern void shader_set_uniform_int(uniform uniform, s32 value);

/* Sets an unsigned integer `uniform` of the current setted shader  to `value`. */
extern void shader_set_uniform_uint(uniform uniform, u32 value);

/* Sets an float `uniform` of the current setted shader to `value`. */
extern void shader_set_uniform_float(uniform uniform, f32 value);

/* Sets an vector 2 `uniform` of the current setted shader to `value`. */
extern void shader_set_uniform_v2(uniform uniform, v2 value);

/* Sets an vector 3 `uniform` of the current setted shader to `value`. */
extern void shader_set_uniform_v3(uniform uniform, v3 value);

/* Sets an vector 4 `uniform` of the current setted shader to `value`. */
extern void shader_set_uniform_v4(uniform uniform, v4 value);

/* Sets an integer array `uniform` of the current setted shader to `values`. */
extern void shader_set_uniform_int_array(uniform uniform, s32 *values, u32 amount);

/* Sets an unsigned integer array `uniform` of the current setted shader to `values`. */
extern void shader_set_uniform_uint_array(uniform uniform, u32 *values, u32 amount);

/* Sets an float array `uniform` of the current setted shader to `values`. */
extern void shader_set_uniform_float_array(uniform uniform, f32 *values, u32 amount);

/* Sets an vector 2 array `uniform` of the current setted shader to `values`. */
extern void shader_set_uniform_v2_array(uniform uniform, v2 *values, u32 amount);

/* Sets an vector 3 array `uniform` of the current setted shader to `values`. */
extern void shader_set_uniform_v3_array(uniform uniform, v3 *values, u32 amount);

/* Sets an vector 4 array `uniform` of the current setted shader to `values`. */
extern void shader_set_uniform_v4_array(uniform uniform, v4 *values, u32 amount);

/*
 * *** Asset Manager
 */

typedef enum {
  ASSET_SHADER,
} asset_type;

/* Loads an asset into memory. */
extern void asset_load(asset_type type, str name);

/* Unloads an asset from memory. */
extern void asset_unload(asset_type type, str name);

/*
 * *** Texture 2D
 */

#if 0
typedef u32 pixel;

typedef enum {
  PIXEL_RGBA,
  PIXEL_BGRA,
} pixel_type;

typedef pixel texture_2d;

typedef enum {
  T2D_NEAREST,
  T2D_LINEAR
} texture_2d_filter_type;

typedef struct {
  texture_2d_filter_type filter_min;
  texture_2d_filter_type filter_mag;
} texture_2d_attributes;

/* Creates a new texture 2d of the dimensions `width`X`height`, with the pixel storage type of `pixel_type`
 * and with the specified `attribs`.
 * If `attribs` is NULL then the default attributes will be used.
 * */
extern texture_2d *texture_2d_create(u32 width, u32 height, pixel_type pixel_type, texture_2d_attributes *attribs);

/* Sets a texture 2d to be the current used by the program.
 * If `tex` is NULL the current setted texture will be unsetted.
 * 
 * Default attributes values:
 *   filter_min = T2D_NEAREST
 *   filter_mag = T2D_NEAREST
 *
 * */
extern void texture_2d_set(texture_2d *tex);

/* Updates the internal texture 2d with the `tex` contents. */
extern void texture_2d_update(texture_2d *tex);

/* Destroys a texture 2d. */
extern void texture_2d_destroy(texture_2d *tex);
#endif

/*
 * *** Rendering ***
 */

enum {
  BATCH_SHADER_QUAD = 0,
  BATCH_SHADER_TEXTURE,
  BATCH_SHADERS_AMOUNT
};

#define DEFAULT_SHADER_QUAD     STR("quad")
#define DEFAULT_SHADER_TEXTURE  STR("texture")

typedef v2 quad_texture_coords[4];

#define COL_WHITE        ((v4) { 1.00f, 1.00f, 1.00f, 1.00f })
#define COL_BLACK        ((v4) { 0.00f, 0.00f, 0.00f, 1.00f })
#define COL_GRAY         ((v4) { 0.50f, 0.50f, 0.50f, 1.00f })
#define COL_LIGHT_GRAY   ((v4) { 0.75f, 0.75f, 0.75f, 1.00f })
#define COL_DARK_GRAY    ((v4) { 0.25f, 0.25f, 0.25f, 1.00f })
#define COL_RED          ((v4) { 1.00f, 0.00f, 0.00f, 1.00f })
#define COL_GREEN        ((v4) { 0.00f, 1.00f, 0.00f, 1.00f })
#define COL_BLUE         ((v4) { 0.00f, 0.00f, 1.00f, 1.00f })
#define COL_YELLOW       ((v4) { 1.00f, 1.00f, 0.00f, 1.00f })
#define COL_MAGENTA      ((v4) { 1.00f, 0.00f, 1.00f, 1.00f })

/* Submits the current rendering batch into the screen. */
void submit_batch(void);

/* Clears the screen with `color` */
extern void clear_screen(v4 color);

/* Draws a quad into the screen */
extern void draw_quad(v2 position, v2 size, v4 blend, u32 layer);

/* Draws the current setted texture 2d into a quad.
 * `texcoords` is for drawing parts of the texture, in case it's NULL it'll draw the full texture. */
extern void draw_texture_2d(v2 position, v2 size, v4 blend, u32 layer, quad_texture_coords *texcoords);

/*
 * A configuration struct to setup the app
 * */
typedef struct {
  cstr title;
  s32  width;
  s32  height;
  b8   center;
  b8   resizable;
  u32  quads_capacity;
  u32  layers_amount;
} blib_config;

#endif/*__BLIB_H__*/
