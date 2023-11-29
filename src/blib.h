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

typedef struct { s32 x, y;       } v2i;
typedef struct { s32 x, y, z;    } v3i;
typedef struct { s32 x, y, z, w; } v4i;

typedef struct { u32 x, y;       } v2u;
typedef struct { u32 x, y, z;    } v3u;
typedef struct { u32 x, y, z, w; } v4u;

#define V2(X, Y)       ((v2){ X, Y       })
#define V3(X, Y, Z)    ((v3){ X, Y, Z    })
#define V4(X, Y, Z, W) ((v4){ X, Y, Z, W })

#define V2I(X, Y)       ((v2i){ X, Y       })
#define V3I(X, Y, Z)    ((v3i){ X, Y, Z    })
#define V4I(X, Y, Z, W) ((v4i){ X, Y, Z, W })

#define V2U(X, Y)       ((v2u){ X, Y       })
#define V3U(X, Y, Z)    ((v3u){ X, Y, Z    })
#define V4U(X, Y, Z, W) ((v4u){ X, Y, Z, W })

#define V2_0 V2(0, 0)
#define V3_0 V3(0, 0, 0)
#define V4_0 V4(0, 0, 0, 0)

#define V2I_0 V2I(0, 0)
#define V3I_0 V3I(0, 0, 0)
#define V4I_0 V4I(0, 0, 0, 0)

#define V2U_0 V2U(0, 0)
#define V3U_0 V3U(0, 0, 0)
#define V4U_0 V4U(0, 0, 0, 0)

static inline v2  v2_add(v2 a, v2 b)  { return V2(a.x+b.x, a.y+b.y);                   }
static inline v2  v2_sub(v2 a, v2 b)  { return V2(a.x-b.x, a.y-b.y);                   }
static inline v2  v2_mul(v2 a, v2 b)  { return V2(a.x*b.x, a.y*b.y);                   }
static inline v2  v2_div(v2 a, v2 b)  { return V2(a.x/b.x, a.y/b.y);                   }
static inline f32 v2_dot(v2 a, v2 b)  { return a.x*b.x + a.y*b.y;                      }
static inline f32 v2_mag(v2 a)        { return sqrtf(a.x*a.x + a.y*a.y);               }
static inline f32 v2_dist(v2 a, v2 b) { return v2_mag(v2_sub(b, a));                   }
static inline v3  v3_add(v3 a, v3 b)  { return V3(a.x+b.x, a.y+b.y, a.z+b.z);          }
static inline v3  v3_sub(v3 a, v3 b)  { return V3(a.x-b.x, a.y-b.y, a.z-b.z);          }
static inline v3  v3_mul(v3 a, v3 b)  { return V3(a.x*b.x, a.y*b.y, a.z*b.z);          }
static inline v3  v3_div(v3 a, v3 b)  { return V3(a.x/b.x, a.y/b.y, a.z/b.z);          }
static inline f32 v3_dot(v3 a, v3 b)  { return a.x*b.x + a.y*b.y + a.z*b.z;            }
static inline f32 v3_mag(v3 a)        { return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);     }
static inline f32 v3_dist(v3 a, v3 b) { return v3_mag(v3_sub(b, a));                   }
static inline v4  v4_add(v4 a, v4 b)  { return V4(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w); }
static inline v4  v4_sub(v4 a, v4 b)  { return V4(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w); }
static inline v4  v4_mul(v4 a, v4 b)  { return V4(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w); }
static inline v4  v4_div(v4 a, v4 b)  { return V4(a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w); }
static inline f32 v4_dot(v4 a, v4 b)  { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;  }
static inline f32 v4_mag(v4 a)        { return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);     }
static inline f32 v4_dist(v4 a, v4 b) { return v4_mag(v4_sub(b, a));                   }

static inline v2i v2i_add(v2i a, v2i b) { return V2I(a.x+b.x, a.y+b.y);                   }
static inline v2i v2i_sub(v2i a, v2i b) { return V2I(a.x-b.x, a.y-b.y);                   }
static inline v2i v2i_mul(v2i a, v2i b) { return V2I(a.x*b.x, a.y*b.y);                   }
static inline v2i v2i_div(v2i a, v2i b) { return V2I(a.x/b.x, a.y/b.y);                   }
static inline f32 v2i_dot(v2i a, v2i b) { return a.x*b.x + a.y*b.y;                       }
static inline f32 v2i_mag(v2i a)        { return sqrtf(a.x*a.x + a.y*a.y);                }
static inline f32 v2i_dist(v2i a, v2i b){ return v2i_mag(v2i_sub(b, a));                  }
static inline v3i v3i_add(v3i a, v3i b) { return V3I(a.x+b.x, a.y+b.y, a.z+b.z);          }
static inline v3i v3i_sub(v3i a, v3i b) { return V3I(a.x-b.x, a.y-b.y, a.z-b.z);          }
static inline v3i v3i_mul(v3i a, v3i b) { return V3I(a.x*b.x, a.y*b.y, a.z*b.z);          }
static inline v3i v3i_div(v3i a, v3i b) { return V3I(a.x/b.x, a.y/b.y, a.z/b.z);          }
static inline f32 v3i_dot(v3i a, v3i b) { return a.x*b.x + a.y*b.y + a.z*b.z;             }
static inline f32 v3i_mag(v3i a)        { return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);      }
static inline f32 v3i_dist(v3i a, v3i b){ return v3i_mag(v3i_sub(b, a));                  }
static inline v4i v4i_add(v4i a, v4i b) { return V4I(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w); }
static inline v4i v4i_sub(v4i a, v4i b) { return V4I(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w); }
static inline v4i v4i_mul(v4i a, v4i b) { return V4I(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w); }
static inline v4i v4i_div(v4i a, v4i b) { return V4I(a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w); }
static inline f32 v4i_dot(v4i a, v4i b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;   }
static inline f32 v4i_mag(v4i a)        { return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);      }
static inline f32 v4i_dist(v4i a, v4i b){ return v4i_mag(v4i_sub(b, a));                  }

static inline v2u v2u_add(v2u a, v2u b) { return V2U(a.x+b.x, a.y+b.y);                   }
static inline v2u v2u_sub(v2u a, v2u b) { return V2U(a.x-b.x, a.y-b.y);                   }
static inline v2u v2u_mul(v2u a, v2u b) { return V2U(a.x*b.x, a.y*b.y);                   }
static inline v2u v2u_div(v2u a, v2u b) { return V2U(a.x/b.x, a.y/b.y);                   }
static inline f32 v2u_dot(v2u a, v2u b) { return a.x*b.x + a.y*b.y;                       }
static inline f32 v2u_mag(v2u a)        { return sqrtf(a.x*a.x + a.y*a.y);                }
static inline f32 v2u_dist(v2u a, v2u b){ return v2u_mag(v2u_sub(b, a));                  }
static inline v3u v3u_add(v3u a, v3u b) { return V3U(a.x+b.x, a.y+b.y, a.z+b.z);          }
static inline v3u v3u_sub(v3u a, v3u b) { return V3U(a.x-b.x, a.y-b.y, a.z-b.z);          }
static inline v3u v3u_mul(v3u a, v3u b) { return V3U(a.x*b.x, a.y*b.y, a.z*b.z);          }
static inline v3u v3u_div(v3u a, v3u b) { return V3U(a.x/b.x, a.y/b.y, a.z/b.z);          }
static inline f32 v3u_dot(v3u a, v3u b) { return a.x*b.x + a.y*b.y + a.z*b.z;             }
static inline f32 v3u_mag(v3u a)        { return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);      }
static inline f32 v3u_dist(v3u a, v3u b){ return v3u_mag(v3u_sub(b, a));                  }
static inline v4u v4u_add(v4u a, v4u b) { return V4U(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w); }
static inline v4u v4u_sub(v4u a, v4u b) { return V4U(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w); }
static inline v4u v4u_mul(v4u a, v4u b) { return V4U(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w); }
static inline v4u v4u_div(v4u a, v4u b) { return V4U(a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w); }
static inline f32 v4u_dot(v4u a, v4u b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;   }
static inline f32 v4u_mag(v4u a)        { return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);      }
static inline f32 v4u_dist(v4u a, v4u b){ return v4u_mag(v4u_sub(b, a));                  }

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
 * *** Asset Manager
 */

typedef enum {
  ASSET_SHADER,
  ASSET_ATLAS
} asset_type;

/* Loads an asset into memory.
 * Asset types:
 *   Shader:
 *     A directory with the shader's name must exists, inside the directory two files
 *     will be searched: vertex.glsl and fragment.glsl.
 *   Atlas:
 *     supported atlases formats:
 *       png
 *       tga
 *       bmp
 *       jpg/jpeg
 *
 * */
extern void asset_load(asset_type type, str name);

/* Unloads an asset from memory. */
extern void asset_unload(asset_type type, str name);

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
 * *** Texture Atlas ***
 */

typedef u32 texture_id;

/* Setups the general info of `atlas`. */
extern void texture_atlas_setup(str atlas, u32 tile_width, u32 tile_height, u32 padding_x, u32 padding_y);

/* Gets the `atlas` texture id. */
extern texture_id texture_atlas_get_id(str atlas);

/*
 * *** Texture Buffer
 */

typedef u32 pixel;

typedef enum {
  T2D_NEAREST,
  T2D_LINEAR
} texture_buff_filter_type;

typedef struct {
  texture_buff_filter_type filter_min;
  texture_buff_filter_type filter_mag;
} texture_buff_attributes;

/* Creates a new texture buffer of the dimensions `width`X`height` and with the specified `attribs`.
 * If `attribs` is NULL then the default attributes will be used.
 * 
 * Default attributes values:
 *   filter_min = T2D_NEAREST
 *   filter_mag = T2D_NEAREST
 *
 * */
extern pixel *texture_buff_create(u32 width, u32 height, texture_buff_attributes *attribs);

/* Updates the texture buffer contents. */
extern void texture_buff_update(pixel *buff);

/* Destroys a texture buffer. */
extern void texture_buff_destroy(pixel *buff);

/*
 * *** Rendering ***
 */

enum {
  BATCH_SHADER_QUAD = 0,
  BATCH_SHADER_TEXTURE,
  BATCH_SHADERS_AMOUNT
};

typedef struct {
  str shaders[BATCH_SHADERS_AMOUNT];
  str atlas;
} batch;

#define DEFAULT_SHADER_QUAD     STR("quad")
#define DEFAULT_SHADER_TEXTURE  STR("texture")

typedef v2 quad_texture_coords[4];

#define COL_WHITE        V4(1.00f, 1.00f, 1.00f, 1.00f)
#define COL_BLACK        V4(0.00f, 0.00f, 0.00f, 1.00f)
#define COL_GRAY         V4(0.50f, 0.50f, 0.50f, 1.00f)
#define COL_LIGHT_GRAY   V4(0.75f, 0.75f, 0.75f, 1.00f)
#define COL_DARK_GRAY    V4(0.25f, 0.25f, 0.25f, 1.00f)
#define COL_RED          V4(1.00f, 0.00f, 0.00f, 1.00f)
#define COL_GREEN        V4(0.00f, 1.00f, 0.00f, 1.00f)
#define COL_BLUE         V4(0.00f, 0.00f, 1.00f, 1.00f)
#define COL_YELLOW       V4(1.00f, 1.00f, 0.00f, 1.00f)
#define COL_MAGENTA      V4(1.00f, 0.00f, 1.00f, 1.00f)

/* Submits the current rendering batch into the screen. */
extern void submit_batch(void);

/* Clears the screen with `color` */
extern void clear_screen(v4 color);

/* Draws a quad into the screen */
extern void draw_quad(v2 position, v2 size, v4 blend, u32 layer);

/* Draws a tile of the current batch texture. */
extern void draw_tile(v2u tile, v2 position, v2 size, v4 blend, u32 layer);

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
