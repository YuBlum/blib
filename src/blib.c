#include "blib.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include <time.h>

#ifdef __linux
#include <uuid/uuid.h>
#endif

#define inf(...) fprintf(stderr, "Info:  " __VA_ARGS__)
#define wrn(...) fprintf(stderr, "Warn:  " __VA_ARGS__)
#define err(...) fprintf(stderr, "Error: " __VA_ARGS__)

/*
 * ****************************
 * ****************************
 * ********* GLOBALS **********
 * ****************************
 * ****************************
 */

/*
 * Camera
 */

static struct {
  m3 proj;
  u32 width;
  u32 height;
  f32 angle;
  v2f position;
  v2f scale;
} camera;

/*
 * Input
 */
#define KEY_CAP 0x100

static struct {
  struct {
    v2f screen_position;
    v2f position;
    v2f scroll;
    b8 buttons_cur[BTN_CAP];
    b8 buttons_prv[BTN_CAP];
    b8 buttons_tick_prv[BTN_CAP];
  } mouse;
  struct {
    b8 keys_cur[KEY_CAP];
    b8 keys_prv[KEY_CAP];
    b8 keys_tick_prv[KEY_CAP];
  } keyboard;
} input;

/*
 * *** Rendering ***
 */

typedef struct {
  v2f position;
  v2f texcoord;
  v4f blend;
} vertex;

typedef vertex quad[4];

static struct {
  u32 quads_amount;
  u32 vertices_capa;
  u32 indices_capa;
  vertex *vertices;

  u32 layers_amount;
  quad ***requests;

  batch batch;

  u32 vao;
  u32 vbo;
  u32 ibo;
} renderer;

/*
 * *** Entity System
 */

typedef struct {
  void *list;
  u32   type;
} entity_component;

typedef struct {
  str *component_names;
  hash_table *components;
  u128 *indexes_ids;
  hash_table *indexes;
  u32 amount;
  u32 name_index;
} entity_type;

static struct {
  str *entity_type_names;
  hash_table *entities;
  entity_type *new_type;
} entity_system;


/*
 * *** Vectors ***
 * */

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

str
string_create(str src) {
  str str;
  str.size = src.size;
  str.capa = str.size + !str.size + (str.size > 0); /* if str.size == 0 then str.capa = 1 else str.capa = str.size + 1 */
  str.buff = malloc(sizeof (char) * str.capa);
  if (str.size) {
    memcpy(str.buff, src.buff, sizeof (char) * src.size);
    str.buff[src.size] = '\0';
  }
  return str;
}

void
string_reserve(str *str, u32 amount) {
  if (!str->capa) {
    wrn("string_reserve(): `str` must have been created by `string_create()`\n");
    return;
  }
  str->capa += amount + 1;
  str->buff = realloc(str->buff, str->capa);
}

void
string_copy(str *dest, str src) {
  if (!dest->capa) {
    wrn("string_copy(): `dest` must have been created by `string_create()`\n");
    return;
  }
  if (!src.size) {
    dest->size = 0;
    dest->buff[0] = '\0';
    return;
  }
  if (dest->capa < src.size) {
    dest->capa += src.size;
    dest->buff = realloc(dest->buff, sizeof (char) * dest->capa);
  }
  memcpy(dest->buff, src.buff, sizeof (char) * src.size);
  dest->size = src.size;
  dest->buff[src.size] = '\0';
}

void
string_concat(str *dest, str src) {
  if (!dest->capa) {
    wrn("string_copy(): `dest` must have been created by `string_create()`\n");
    return;
  }
  if (!src.size) return;
  if (dest->size + src.size + 1 > dest->capa) {
    dest->capa += src.size + 1;
    dest->buff = realloc(dest->buff, sizeof (char) * dest->capa);
  }
  memcpy(dest->buff + dest->size, src.buff, sizeof (char) * src.size);
  dest->size += src.size;
  dest->buff[dest->size] = '\0';
}

b8
string_equal(str s1, str s2) {
  if (s1.size != s2.size) return false;
  for (u32 i = 0; i < s1.size; i++) {
    if (s1.buff[i] != s2.buff[i]) return false;
  }
  return true;
}

str
string_view(str src, u32 start, u32 end) {
  if (start > end) {
    wrn("string_view(): `end` must be greater or equal to `start`\n");
    return STR_0;
  }
  if (end >= src.size) {
    wrn("string_view(): `end`(%u) is out of bounds on `src` size(%u)\n", end, src.size);
    return STR_0;
  }
  return (str) {
    .size = end - start + 1,
    .buff = src.buff + start,
    .capa = 0
  };
}

str
string_sub(str src, u32 start, u32 end) {
  return string_create(string_view(src, start, end));
}

void
string_insert(str *dest, str src, u32 index) {
  if (!dest->capa) {
    wrn("string_insert(): `dest` must have been created by `string_create()`\n");
    return;
  }
  if (!src.size) return;
  if (index >= dest->size) {
    wrn("string_insert(): `index`(%u) is out of bounds on `dest` size(%u)\n", index, dest->size);
    return;
  }
  if (dest->size + src.size + 1 > dest->capa) {
    dest->capa += src.size + 1;
    dest->buff = realloc(dest->buff, sizeof (char) * dest->capa);
  }
  memmove(dest->buff + index + src.size, dest->buff + index, dest->size - index);
  memcpy(dest->buff + index, src.buff, src.size);
  dest->size += src.size;
  dest->buff[dest->size] = '\0';
}

s8 *
string_find_first(str str, s8 c) {
  for (u32 i = 0; i < str.size; i++) {
    if (str.buff[i] == c) return (s8 *)(str.buff + i);
  }
  return 0;
}

s8 *
string_find_last(str str, s8 c) {
  for (u32 i = str.size - 1; i != (u32)-1; i--) {
    if (str.buff[i] == c) return (s8 *)(str.buff + i);
  }
  return 0;
}

void
string_reverse(str str) {
  if (!str.capa) {
    wrn("string_reverse(): `str` must have been created by `string_create()`\n");
    return;
  }
  cstr start = str.buff;
  cstr end   = str.buff + str.size - 1;
  while (start < end) {
    *start ^= *end;
    *end   ^= *start;
    *start ^= *end;
    start++;
    end--;
  }
}

void
string_destroy(str str) {
  if (!str.capa) {
    wrn("string_destroy(): `str` must have been created by `string_create()`\n");
    return;
  }
  free(str.buff);
}

/*
 * 
 * *** Array List ***
 *
 * */

typedef struct {
  u32 size;
  u32 capa;
  u32 type;
} array_list_header;
#define ARRAY_LIST_HEADER(ARR) (((array_list_header *)ARR) - 1)

void *
array_list_create(u32 type_size) {
  array_list_header *header = malloc(sizeof (array_list_header) + type_size);
  header->type = type_size;
  header->size = 0;
  header->capa = 1;
  return header + 1;
}

u32
array_list_capacity(void *arr) {
  return ARRAY_LIST_HEADER(arr)->capa;
}

u32
array_list_size(void *arr) {
  return ARRAY_LIST_HEADER(arr)->size;
}

void *
array_list_reserve(void *arr, u32 amount) {
  array_list_header *header = ARRAY_LIST_HEADER(arr);
  header->capa += amount;
  header = realloc(header, sizeof (array_list_header) + header->type * header->capa);
  return header + 1;
}

void *
array_list_grow(void *arr, u32 amount) {
  array_list_header *header = ARRAY_LIST_HEADER(arr);
  header->size += amount;
  if (header->size >= header->capa) {
    header->capa *= 2;
    header = realloc(header, sizeof (array_list_header) + header->type * header->capa);
  }
  return header + 1;
}

void
array_list_shift_left(void *arr, u32 index, u32 amount) {
  if (amount == 0) return;
  array_list_header *header = ARRAY_LIST_HEADER(arr);
  if (amount > index) {
    index = amount;
  }
  if (index >= header->size) return;
  memmove(
      (u8 *)arr + (index - amount) * header->type,
      (u8 *)arr +  index           * header->type,
      (header->size - index) * header->type);
}

void
array_list_shift_right(void *arr, u32 index, u32 amount) {
  if (amount == 0) return;
  array_list_header *header = ARRAY_LIST_HEADER(arr);
  if (index + amount >= header->size) return;
  memmove(
      (u8 *)arr + (index + amount) * header->type,
      (u8 *)arr +  index           * header->type,
      (header->size - (index + amount)) * header->type);
}

#if 0
void *
array_list_push(void *arr, void *item) {
  arr = array_list_grow(arr, 1);
  array_list_header *header = ARRAY_LIST_HEADER(arr);
  memcpy((u8 *)arr + header->type * (header->size - 1), item, header->type);
  return arr;
}

void *
array_list_insert(void *arr, u32 index, void *item) {
  array_list_header *header = ARRAY_LIST_HEADER(arr);
  u32 max_index = header->size > 0 ? header->size - 1 : 0;
  if (index > max_index) {
    arr = array_list_grow(arr, index - max_index);
    header = ARRAY_LIST_HEADER(arr);
  } else {
    arr = array_list_grow(arr, 1);
    header = ARRAY_LIST_HEADER(arr);
    memmove((u8 *)arr + header->type * (index + 1),
            (u8 *)arr + header->type * (index),
            (header->size - index) * header->type);
  }
  memcpy((u8 *)arr + header->type * index, item, header->type);
  return arr;
}
#endif

void
array_list_pop(void *arr, void *out) {
  array_list_header *header = ARRAY_LIST_HEADER(arr);
  if (header->size == 0) return;
  header->size--;
  if (out)
    memcpy(out, (u8 *)arr + header->type * header->size, header->type);
}

void
array_list_remove(void *arr, u32 index, void *out) {
  array_list_header *header = ARRAY_LIST_HEADER(arr);
  if (header->size == 0 || index >= header->size) return;
  if (out) memcpy(out, (u8 *)arr + header->type * index, header->type);
  array_list_shift_left(arr, index + 1, 1);
  header->size--;
}

void
array_list_clear(void *arr) {
  array_list_header *header = ARRAY_LIST_HEADER(arr);
   header->size = 0;
}

void
array_list_destroy(void *arr) {
  free(ARRAY_LIST_HEADER(arr));
}

/*
 *
 * *** Hash Table ***
 *
 * */

static u32 hash_table_key_size[] = {
  [HT_STR] = sizeof (str),
  [HT_U64] = sizeof (u64),
  [HT_U128] = sizeof (u128)
};

typedef union {
  void *ptr;
  str *str;
  u64 *u64;
  u128 *u128;
} hash_table_key;

struct hash_table {
  hash_table_type key_type;
  hash_table_key  keys;
  b8             *free;
  u8             *vals;
  void           *buff;
  u32             type;
  u32             capa;
  u32             size;
};

#define INITITAL_HASH_TABLE_CAP 17

static u32
hash_str(str s) {
  u32 hash = 5381;
  for (u32 i = 0; i < s.size; i++) {
    hash = ((hash << 5) + hash) + s.buff[i]; /* hash * 33 + c */
  }
  return hash;
}

static u32
hash_u64(u64 x) {
  x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
  x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
  x = x ^ (x >> 31);
  return x;
}

static u32
hash_u128(u128 x) {
  return hash_u64(x.u64[0]) | hash_u64(x.u64[1]);
}

static u32
hash(hash_table *ht, void *key) {
  switch (ht->key_type) {
    case HT_STR:
      return hash_str(*(str *)key);
    case HT_U64:
      return hash_u64(*(u64 *)key);
    case HT_U128:
      return hash_u128(*(u128 *)key);
    case HT_AMOUNT:
      err("hash(): unreachable\n");
      exit(1);
  }
  return 0;
}

hash_table *
hash_table_create(u32 type_size, hash_table_type key_type) {
  hash_table *ht = malloc(sizeof (hash_table));
  ht->key_type = key_type;
  ht->capa = INITITAL_HASH_TABLE_CAP;
  ht->size = 0;
  ht->type = type_size;
  ht->buff = malloc((sizeof (b8) + hash_table_key_size[key_type] + type_size) * ht->capa);
  ht->free     = (b8 *)ht->buff;
  ht->keys.ptr = (u8 *)ht->free     + sizeof (b8)                   * ht->capa;
  ht->vals     = (u8 *)ht->keys.ptr + hash_table_key_size[key_type] * ht->capa;
  memset(ht->free, true, sizeof (b8) * ht->capa);
  return ht;
}

void *
hash_table_get(hash_table *ht, void *key) {
  u32 index = hash(ht, key) % ht->capa;
  while (!ht->free[index]) {
    switch (ht->key_type) {
      case HT_STR:
        if (string_equal(ht->keys.str[index], *(str *)key))
          return ht->vals + (index * ht->type);
        break;
      case HT_U64:
        if (ht->keys.u64[index] == *(u64 *)key)
          return ht->vals + (index * ht->type);
        break;
      case HT_U128:
        if (ht->keys.u128[index].u64[0] == ((u128 *)key)->u64[0] &&
            ht->keys.u128[index].u64[1] == ((u128 *)key)->u64[1])
          return ht->vals + (index * ht->type);
        break;
      case HT_AMOUNT: break;
    }
    index = (index + 1) % ht->capa;
  }
  return 0;
}

void
hash_table_value_key(hash_table *ht, void *val, void *out_key) {
  if (ht->vals > (u8 *)val || ht->vals + ht->capa * ht->type <= (u8 *)val) {
    wrn("hash_table_value_key(): invalid value\n");
    return;
  }
  u32 index = (u8 *)val - ht->vals;
  if ((index % ht->type) != 0) {
    wrn("hash_table_value_key(): invalid value\n");
    return;
  }
  index /= ht->type;
  memcpy(out_key, (u8 *)ht->keys.ptr + index * hash_table_key_size[ht->key_type], hash_table_key_size[ht->key_type]);
}

void *
hash_table_add(hash_table *ht, void *key) {
  if (!key) {
    wrn("hash_table_add(): the key cannot be NULL\n");
    return 0;
  }
  f32 load_factor = (f32)ht->size / (f32)ht->capa;
  if (load_factor >= 0.5f) {
    u32 old_capa = ht->capa;
    ht->capa *= 2;
    void *old_buff = malloc((sizeof (str) + ht->type) * old_capa);
    memcpy(old_buff, ht->buff, (sizeof (str) + ht->type) * old_capa);
    b8             *old_free = old_buff;
    hash_table_key  old_keys = { (u8 *)old_free     + sizeof (b8)                       * ht->capa };
    u8             *old_vals =   (u8 *)old_keys.ptr + hash_table_key_size[ht->key_type] * old_capa;

    ht->buff = realloc(ht->buff, (sizeof (b8) + hash_table_key_size[ht->key_type] + ht->type) * ht->capa);
    ht->free     = (b8 *)ht->buff;
    ht->keys.ptr = (u8 *)ht->free     + sizeof (b8)                       * ht->capa;
    ht->vals     = (u8 *)ht->keys.ptr + hash_table_key_size[ht->key_type] * ht->capa;
    memset(ht->free, true, sizeof (b8) * ht->capa);

    for (u32 i = 0; i < old_capa; i++) {
      if (!old_free[i]) continue;
      u32 index = hash(ht, (u8 *)old_keys.ptr + i * hash_table_key_size[ht->key_type]) % ht->capa;
      while (!ht->free[index]) {
        index = (index + 1) % ht->capa;
      }
      switch (ht->key_type) {
        case HT_STR:
          ht->keys.str[index] = string_create(old_keys.str[i]);
          break;
        case HT_U64:
          ht->keys.u64[index] = old_keys.u64[i];
          break;
        case HT_U128:
          ht->keys.u128[index] = old_keys.u128[i];
          break;
        case HT_AMOUNT: break;
      }
      memcpy(ht->vals + ht->type * index, old_vals + ht->type * i, ht->type);
      ht->free[index] = false;
    }

    free(old_buff);
  }

  u32 index = hash(ht, key) % ht->capa;
  while (!ht->free[index]) {
    switch (ht->key_type) {
      case HT_STR:
        if (string_equal(ht->keys.str[index], *(str *)key)) {
          wrn("hash_table_add(): the key '%.*s' is already on the hash table\n", ((str *)key)->size, ((str *)key)->buff);
          return 0;
        }
        break;
      case HT_U64:
        if (ht->keys.u64[index] == *(u64 *)key) {
          wrn("hash_table_add(): the key '%lu' is already on the hash table\n", *(u64 *)key);
          return 0;
        }
        break;
      case HT_U128:
        if (ht->keys.u128[index].u64[0] == ((u128 *)key)->u64[0] &&
            ht->keys.u128[index].u64[1] == ((u128 *)key)->u64[1]) {
          wrn("hash_table_add(): the key '%lu%lu' is already on the hash table\n", ((u128 *)key)->u64[1], ((u128 *)key)->u64[0]);
          return 0;
        }
        break;
      case HT_AMOUNT: break;
    }
    index = (index + 1) % ht->capa;
  }
  switch (ht->key_type) {
    case HT_STR:
      ht->keys.str[index] = string_create(*(str *)key);
      break;
    case HT_U64:
      ht->keys.u64[index] = *(u64 *)key;
      break;
    case HT_U128:
      ht->keys.u128[index] = *(u128 *)key;
      break;
    case HT_AMOUNT: break;
  }
  ht->size++;
  ht->free[index] = false;
  return ht->vals + index * ht->type;
}

void
hash_table_del(hash_table *ht, void *key) {
  u32 index = hash(ht, key) % ht->capa;
  while (!ht->free[index]) {
    switch (ht->key_type) {
      case HT_STR:
        if (string_equal(ht->keys.str[index], *(str *)key)) {
          string_destroy(ht->keys.str[index]);
          ht->free[index] = true;
          ht->size--;
          return;
        }
        break;
      case HT_U64:
        if (ht->keys.u64[index] == *(u64 *)key) {
          ht->free[index] = true;
          ht->size--;
          return;
        }
        break;
      case HT_U128:
        if (ht->keys.u128[index].u64[0] == ((u128 *)key)->u64[0] &&
            ht->keys.u128[index].u64[1] == ((u128 *)key)->u64[1]) {
          ht->free[index] = true;
          ht->size--;
          return;
        }
        break;
      case HT_AMOUNT: break;
    }
    index = (index + 1) % ht->capa;
  }
  switch (ht->key_type) {
    case HT_STR:
      if (string_equal(ht->keys.str[index], *(str *)key))
        wrn("hash_table_del(): the key '%.*s' doesn't exists", ((str *)key)->size, ((str *)key)->buff);
      return;
    case HT_U64:
      if (ht->keys.u64[index] == *(u64 *)key)
        wrn("hash_table_del(): the key '%lu' doesn't exists", *(u64 *)key);
      return;
    case HT_U128:
      if (ht->keys.u128[index].u64[0] == ((u128 *)key)->u64[0] &&
          ht->keys.u128[index].u64[1] == ((u128 *)key)->u64[1])
        wrn("hash_table_del(): the key '%lu' doesn't exists", *(u64 *)key);
      return;
    case HT_AMOUNT: return;
  }
}

void
hash_table_clear(hash_table *ht) {
  ht->size = 0;
  memset(ht->free, 0, ht->capa);
}

void
hash_table_destroy(hash_table *ht) {
  switch (ht->key_type) {
    case HT_STR:
      for (u32 i = 0; i < ht->capa; i++) {
        if (ht->free[i]) continue;
        string_destroy(ht->keys.str[i]);
      }
      break;
    case HT_U64:     break;
    case HT_U128:    break;
    case HT_AMOUNT:  break;
  }
  free(ht->buff);
  free(ht);
}

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

static void
entity_system_init(void) {
  entity_system.new_type          = 0;
  entity_system.entities          = hash_table_create(sizeof (entity_type), HT_STR);
  entity_system.entity_type_names = array_list_create(sizeof (str));
}

void
entity_type_begin(str name) {
  if (entity_system.new_type) {
    wrn("entity_type_begin(): trying to create two entity types at the same time. forgot entity_type_end()?\n");
    return;
  }
  if (!name.size) {
    wrn("entity_type_begin(): entity type name can't be empty\n");
    return;
  }
  if (hash_table_get(entity_system.entities, &name)) {
    wrn("entity_type_begin(): entity type '%.*s' already exists\n", name.size, name.buff);
    return;
  }
  entity_system.new_type = hash_table_add(entity_system.entities, &name);
  entity_system.new_type->component_names = array_list_create(sizeof (str));
  entity_system.new_type->components      = hash_table_create(sizeof (entity_component), HT_STR);
  entity_system.new_type->indexes_ids     = array_list_create(sizeof (u128));
  entity_system.new_type->indexes         = hash_table_create(sizeof (u32), HT_U128);
  entity_system.new_type->name_index      = array_list_size(entity_system.entity_type_names);
  entity_system.new_type->amount          = 0;
  array_list_push(entity_system.entity_type_names, string_create(name));
}

void
entity_type_add_component(str name, u32 size) {
  if (!entity_system.new_type) {
    wrn("entity_type_add_component(): trying to create add component without initiating a new entity type. forgot entity_type_begin()?\n");
    return;
  }
  if (!name.size) {
    wrn("entity_type_add_component(): entity component name can't be empty\n");
    return;
  }
  if (hash_table_get(entity_system.new_type->components, &name)) {
    str entity_type_name;
    hash_table_value_key(entity_system.entities, entity_system.new_type, &entity_type_name);
    wrn("entity_type_add_component(): component '%.*s' already exists on entity '%.*s'\n",
        name.size, name.buff, entity_type_name.size, entity_type_name.buff);
    return;
  }
  entity_component *component = hash_table_add(entity_system.new_type->components, &name);
  component->list = array_list_create(size);
  component->type = size;
  array_list_push(entity_system.new_type->component_names, string_create(name));
}

void
entity_type_end(void) {
  entity_system.new_type = 0;
}

void *
entity_type_get_components(str type_name, str comp_name) {
  entity_type *type = hash_table_get(entity_system.entities, &type_name);
  if (!type) {
    wrn("entity_type_get_components(): invalid type '%.*s'\n", type_name.size, type_name.buff);
    return 0;
  }
  entity_component *component = hash_table_get(type->components, &comp_name);
  if (!component) {
    wrn("entity_type_get_components(): unexisting component '%.*s'\n", comp_name.size, comp_name.buff);
    return 0;
  }
  return (u8 *)component->list;
}

void
entity_type_clear(str name) {
  entity_type *type = hash_table_get(entity_system.entities, &name);
  if (!type) {
    wrn("entity_type_get_components(): invalid type '%.*s'\n", name.size, name.buff);
    return;
  }
  array_list_clear(type->indexes_ids);
  hash_table_clear(type->indexes);
  type->amount = 0;
}

void
entity_create(str type_name, entity *e) {
  entity_type *type = hash_table_get(entity_system.entities, &type_name);
  if (!type) {
    wrn("entity_create(): type '%.*s' doesn't exists\n", type_name.size, type_name.buff);
    return;
  }

  e->type = type->name_index;

#ifdef __linux
  uuid_generate((u8 *)&e->id);
#endif

  u32 *index = hash_table_add(type->indexes, &e->id);
  if (!index) {
    err("entity_create(): unreachable\n");
    return;
  }
  *index = type->amount++;
  for (u32 i = 0; i < array_list_size(type->component_names); i++) {
    entity_component *component = hash_table_get(type->components, &type->component_names[i]);
    component->list = array_list_grow(component->list, 1);
  }
  array_list_push(type->indexes_ids, e->id);
}

void *
entity_get_component(entity *e, str comp_name) {
  entity_type *type = hash_table_get(entity_system.entities, &entity_system.entity_type_names[e->type]);
  if (!type) {
    wrn("entity_get_component(): entity with invalid type\n");
    return 0;
  }
  entity_component *component = hash_table_get(type->components, &comp_name);
  if (!component) {
    wrn("entity_get_component(): unexisting component '%.*s'\n", comp_name.size, comp_name.buff);
    return 0;
  }
  u32 *index = hash_table_get(type->indexes, &e->id);
  if (!index) {
    wrn("entity_get_component(): entity doesn't exists\n");
    return 0;
  }
  return (u8 *)component->list + (*index) * component->type;
}

void
entity_destroy(entity *e) {
  entity_type *type = hash_table_get(entity_system.entities, &entity_system.entity_type_names[e->type]);
  if (!type) {
    wrn("entity_destroy(): entity with invalid type\n");
    return;
  }
  u32 *index = hash_table_get(type->indexes, &e->id);
  if (!index) {
    wrn("entity_get_component(): entity doesn't exists\n");
    return;
  }
  for (u32 i = (*index) + 1; i < array_list_size(type->indexes_ids); i++) {
    (*(u32 *)hash_table_get(type->indexes, &type->indexes_ids[i]))--;
  }
  array_list_remove(type->indexes_ids, *index, 0);
  for (u32 i = 0; i < array_list_size(type->component_names); i++) {
    entity_component *component = hash_table_get(type->components, &type->component_names[i]);
    array_list_remove(component->list, *index, 0);
  }
  hash_table_del(type->indexes, &e->id);
  type->amount--;
}

/*
 * *** Asset Manager
 */

typedef u32 shader_id;

typedef struct {
  shader_id id;
  b8 use_camera_projection;
  uniform u_camera;
} shader_data;

typedef struct {
  texture_id id;
  u32 width;
  u32 height;
  v2f pixel_size;
  v2f tile_padding;
  v2f tile_size;
  v2f tile_size_px;
} texture_atlas;

typedef struct {
  texture_id id;
  u32 width;
  u32 height;
  v2f pixel_size;
  v2f char_sprite_padding;
  v2f char_padding;
  v2f char_size;
  v2f char_size_px;
} sprite_font;

static struct {
  hash_table *shaders;
  hash_table *atlases;
  hash_table *sprite_fonts;
  str path;
} asset_manager;

enum {
  IMAGE_FORMAT_PNG = 0,
  IMAGE_FORMAT_JPG,
  IMAGE_FORMAT_JPEG,
  IMAGE_FORMAT_TGA,
  IMAGE_FORMAT_BMP,
  IMAGE_FORMAT_AMOUNT
};

enum {
  SHADER_CREATE_RESULT_SUCCESS = 0,
  SHADER_CREATE_RESULT_INVALID_TYPE,
  SHADER_CREATE_RESULT_INVALID_PATH,
};

struct image_format {
  str extension;
  u32 format;
} image_formats[] = {
  [IMAGE_FORMAT_PNG]  = { CONST_STR(".png"),  GL_RGBA },
  [IMAGE_FORMAT_JPG]  = { CONST_STR(".jpg"),  GL_RGB  },
  [IMAGE_FORMAT_JPEG] = { CONST_STR(".jpeg"), GL_RGB  },
  [IMAGE_FORMAT_TGA]  = { CONST_STR(".tga"),  GL_BGRA },
  [IMAGE_FORMAT_BMP]  = { CONST_STR(".bmp"),  GL_RGB  }
};

typedef struct {
  shader_id sh;
  u32 exit;
} shader_create_result;

typedef struct {
  texture_id tex;
  u32 width;
  u32 height;
  b8 founded;
} image_create_result;

static void
asset_manager_init(void) {
  asset_manager.shaders      = hash_table_create(sizeof (shader_data),   HT_STR);
  asset_manager.atlases      = hash_table_create(sizeof (texture_atlas), HT_STR);
  asset_manager.sprite_fonts = hash_table_create(sizeof (sprite_font),   HT_STR);
  asset_manager.path         = string_create(STR_0);
  string_reserve(&asset_manager.path, 1024);
}

static shader_create_result
asset_manager_shader_create(str name, str path, GLenum type) {
  shader_create_result result = {
    .exit = SHADER_CREATE_RESULT_SUCCESS
  };

  if (type != GL_FRAGMENT_SHADER && type != GL_VERTEX_SHADER) {
    result.exit = SHADER_CREATE_RESULT_INVALID_TYPE;
    return result;
  }

  FILE *sh_file = fopen(path.buff, "r");
  if (!sh_file) {
    result.exit = SHADER_CREATE_RESULT_INVALID_PATH;
    return result;
  }
  fseek(sh_file, 0, SEEK_END);
  u32 sh_siz = ftell(sh_file);
  cstr sh_src = malloc(sh_siz + 1);
  fseek(sh_file, 0, SEEK_SET);
  fread(sh_src, 1, sh_siz, sh_file);
  sh_src[sh_siz] = '\0';

  result.sh = glCreateShader(type);
  glShaderSource(result.sh, 1, (ccstr *)&sh_src, 0);
  glCompileShader(result.sh);

  s32 status;
  glGetShaderiv(result.sh, GL_COMPILE_STATUS, &status);
  if (!status) {
    s32 log_size;
    glGetShaderiv(result.sh, GL_INFO_LOG_LENGTH, &log_size);
    cstr log = malloc(log_size);
    glGetShaderInfoLog(result.sh, log_size, 0, log);
    err("OpenGL: '%.*s' %s shader: %.*s\n", name.size,  name.buff, type == GL_FRAGMENT_SHADER ? "fragment" : "vertex", log_size, log);
    exit(1);
  }

  return result;
}

static image_create_result
asset_manager_image_create(str dir, str name) {
  image_create_result result;
  result.founded = false;
  for (u32 i = 0; i < IMAGE_FORMAT_AMOUNT; i++) {
    asset_manager.path.size = 0;
    string_copy(&asset_manager.path, STR("assets/"));
    string_concat(&asset_manager.path, dir);
    string_concat(&asset_manager.path, name);
    string_concat(&asset_manager.path, image_formats[i].extension);
    s32 n;
    u8 *data = stbi_load(asset_manager.path.buff, (s32 *)&result.width, (s32 *)&result.height,
        &n, 0);
    if (!data) continue;

    glGenTextures(1, &result.tex);
    glBindTexture(GL_TEXTURE_2D, result.tex);
    glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, result.width, result.height, 0,
        image_formats[i].format, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
    result.founded = true;
    break;
  }
  return result;
}

void
asset_load(asset_type type, str name) {
  switch (type) {
    case ASSET_SHADER:
    {
      shader_data *shader = hash_table_add(asset_manager.shaders, &name);
      if (!shader) {
        err("asset_load(): shader with the name '%.*s' is already loaded.\n", name.size, name.buff);
        exit(1);
      }

      shader_id vertex, fragment;
      shader_create_result shader_create_result;

      asset_manager.path.size = 0;
      string_copy(&asset_manager.path, STR("assets/"));
      string_concat(&asset_manager.path, STR("shaders/"));
      string_concat(&asset_manager.path, name);
      string_concat(&asset_manager.path, STR("/vertex.glsl"));
      shader_create_result = asset_manager_shader_create(name, asset_manager.path, GL_VERTEX_SHADER);
      switch (shader_create_result.exit) {
        case SHADER_CREATE_RESULT_INVALID_TYPE:
          err("vertex unreachable");
          exit(1);
        case SHADER_CREATE_RESULT_INVALID_PATH:
          err("asset_load(): vertex shader of '%.*s' doesn't exists.\n", name.size, name.buff);
          exit(1);
      }
      vertex = shader_create_result.sh;
      asset_manager.path.size = 0;
      string_copy(&asset_manager.path, STR("assets/"));
      string_concat(&asset_manager.path, STR("shaders/"));
      string_concat(&asset_manager.path, name);
      string_concat(&asset_manager.path, STR("/fragment.glsl"));
      shader_create_result = asset_manager_shader_create(name, asset_manager.path, GL_FRAGMENT_SHADER);
      switch (shader_create_result.exit) {
        case SHADER_CREATE_RESULT_INVALID_TYPE:
          err("fragment unreachable");
          exit(1);
        case SHADER_CREATE_RESULT_INVALID_PATH:
          err("asset_load(): fragment shader of '%.*s' doesn't exists.\n", name.size, name.buff);
          exit(1);
      }
      fragment = shader_create_result.sh;

      shader->use_camera_projection = false;
      shader->u_camera = -1;
      shader->id = glCreateProgram();
      glAttachShader(shader->id, vertex);
      glAttachShader(shader->id, fragment);
      glLinkProgram(shader->id);

      s32 status;
      glGetProgramiv(shader->id, GL_LINK_STATUS, &status);
      if (!status) {
        s32 log_size;
        glGetShaderiv(shader->id, GL_INFO_LOG_LENGTH, &log_size);
        cstr log = malloc(log_size);
        glGetProgramInfoLog(shader->id, log_size, 0, log);
        err("OpenGL: '%.*s' linker: %.*s\n", name.size,  name.buff, log_size, log);
        exit(1);
      }
      glDeleteShader(vertex);
      glDeleteShader(fragment);
    } break;
    case ASSET_ATLAS:
    {
      texture_atlas *atlas = hash_table_add(asset_manager.atlases, &name);
      if (!atlas) {
        err("asset_load(): texture atlas with the name '%.*s' is already loaded.\n", name.size, name.buff);
        exit(1);
      }

      image_create_result img = asset_manager_image_create(STR("atlases/"), name);
      if (!img.founded) {
        err("asset_load(): texture atlas '%.*s' doesn't exists.\n", name.size, name.buff);
        exit(1);
      }
      atlas->id     = img.tex;
      atlas->width  = img.width;
      atlas->height = img.height;

      atlas->pixel_size = V2F(
        1.0f / (f32)atlas->width,
        1.0f / (f32)atlas->height
      );
      atlas->tile_size_px = V2F(16, 16);
      atlas->tile_size = v2f_mul(atlas->pixel_size, atlas->tile_size_px);
      atlas->tile_padding = V2F_0;
    } break;
    case ASSET_SPRITE_FONT:
    {
      sprite_font *font = hash_table_add(asset_manager.sprite_fonts, &name);
      if (!font) {
        err("asset_load(): sprite font with the name '%.*s' is already loaded.\n", name.size, name.buff);
        exit(1);
      }

      image_create_result img = asset_manager_image_create(STR("spritefonts/"), name);
      if (!img.founded) {
        err("asset_load(): sprite font '%.*s' doesn't exists.\n", name.size, name.buff);
        exit(1);
      }
      font->id     = img.tex;
      font->width  = img.width;
      font->height = img.height;

      font->pixel_size = V2F(
        1.0f / (f32)font->width,
        1.0f / (f32)font->height
      );
      font->char_size_px        = V2F(8, 8);
      font->char_size           = v2f_mul(font->pixel_size, font->char_size_px);
      font->char_padding        = V2F(0, 0);
      font->char_sprite_padding = V2F(0, 0);
    } break;
  }
}

void
asset_unload(asset_type type, str name) {
  switch (type) {
    case ASSET_SHADER:
    {
      shader_data *shader = hash_table_get(asset_manager.shaders, &name);
      if (!shader) {
        wrn("asset_unload(): already unloaded shader '%.*s'.\n", name.size, name.buff);
        return;
      }
      glDeleteProgram(shader->id);
      hash_table_del(asset_manager.shaders, &name);
    } break;
    case ASSET_ATLAS:
    {
      texture_atlas *tex = hash_table_get(asset_manager.atlases, &name);
      if (!tex) {
        wrn("asset_unload(): already unloaded atlas '%.*s'.\n", name.size, name.buff);
        return;
      }
      glDeleteTextures(1, &tex->id);
      hash_table_del(asset_manager.atlases, &name);
    } break;
    case ASSET_SPRITE_FONT:
    {
      sprite_font *font = hash_table_get(asset_manager.sprite_fonts, &name);
      if (!font) {
        wrn("asset_unload(): already unloaded sprite font '%.*s'.\n", name.size, name.buff);
        return;
      }
      glDeleteTextures(1, &font->id);
      hash_table_del(asset_manager.sprite_fonts, &name);
    } break;
  }
}

/*
 * *** Shader ***
 * */

#define SHADER_GET(FUNC, SHADER, NAME) do { \
  (SHADER) = hash_table_get(asset_manager.shaders, &(NAME));\
  if (!(SHADER)) {\
    err("%s(): shader '%.*s' isn't loaded.\n", #FUNC, (NAME).size, (NAME).buff);\
    exit(1);\
  }\
} while (0)

void
shader_use_camera(str shader_name, b8 use) {
  shader_data *shader;
  SHADER_GET(shader_get_uniform, shader, shader_name);
  shader->use_camera_projection = use;
  if (use) {
    shader->u_camera = shader_get_uniform(shader_name, STR("u_camera"));
  }
}

uniform
shader_get_uniform(str shader_name, str uniform_name) {
  shader_data *shader;
  SHADER_GET(shader_get_uniform, shader, shader_name);
  uniform uniform = glGetUniformLocation(shader->id, uniform_name.buff);
  if (uniform == -1) {
    err("shader_get_uniform(): uniform '%.*s' of shader '%.*s' doesn't exists\n",
        uniform_name.size, uniform_name.buff, shader_name.size, shader_name.buff);
    exit(1);
  }
  return uniform;
}

void
shader_set_uniform_int(uniform uniform, s32 value) {
  glUniform1i(uniform, value);
}

void
shader_set_uniform_uint(uniform uniform, u32 value) {
  glUniform1ui(uniform, value);
}

void
shader_set_uniform_float(uniform uniform, f32 value) {
  glUniform1f(uniform, value);
}

void
shader_set_uniform_v2f(uniform uniform, v2f value) {
  glUniform2f(uniform, value.x, value.y);
}

void
shader_set_uniform_v3f(uniform uniform, v3f value) {
  glUniform3f(uniform, value.x, value.y, value.z);
}

void
shader_set_uniform_v4f(uniform uniform, v4f value) {
  glUniform4f(uniform, value.x, value.y, value.z, value.w);
}

void
shader_set_uniform_v2i(uniform uniform, v2i value) {
  glUniform2i(uniform, value.x, value.y);
}

void
shader_set_uniform_v3i(uniform uniform, v3i value) {
  glUniform3i(uniform, value.x, value.y, value.z);
}

void
shader_set_uniform_v4i(uniform uniform, v4i value) {
  glUniform4i(uniform, value.x, value.y, value.z, value.w);
}

void
shader_set_uniform_v2u(uniform uniform, v2u value) {
  glUniform2ui(uniform, value.x, value.y);
}

void
shader_set_uniform_v3u(uniform uniform, v3u value) {
  glUniform3ui(uniform, value.x, value.y, value.z);
}

void
shader_set_uniform_v4u(uniform uniform, v4u value) {
  glUniform4ui(uniform, value.x, value.y, value.z, value.w);
}

void
shader_set_uniform_m2(uniform uniform, m2 value) {
  glUniformMatrix2fv(uniform, 1, GL_FALSE, (f32 *)&value);
}

void
shader_set_uniform_m3(uniform uniform, m3 value) {
  glUniformMatrix3fv(uniform, 1, GL_FALSE, (f32 *)&value);
}

void
shader_set_uniform_m4(uniform uniform, m4 value) {
  glUniformMatrix4fv(uniform, 1, GL_FALSE, (f32 *)&value);
}

void
shader_set_uniform_int_array(uniform uniform, s32 *values, u32 amount) {
  glUniform1iv(uniform, amount, values);
}

void
shader_set_uniform_uint_array(uniform uniform, u32 *values, u32 amount) {
  glUniform1uiv(uniform, amount, values);
}

void
shader_set_uniform_float_array(uniform uniform, f32 *values, u32 amount) {
  glUniform1fv(uniform, amount, values);
}

void
shader_set_uniform_v2f_array(uniform uniform, v2f *values, u32 amount) {
  glUniform2fv(uniform, amount, (f32 *)values);
}

void
shader_set_uniform_v3f_array(uniform uniform, v3f *values, u32 amount) {
  glUniform3fv(uniform, amount, (f32 *)values);
}

void
shader_set_uniform_v4f_array(uniform uniform, v4f *values, u32 amount) {
  glUniform4fv(uniform, amount, (f32 *)values);
}

void
shader_set_uniform_v2i_array(uniform uniform, v2i *values, u32 amount) {
  glUniform2iv(uniform, amount, (s32 *)values);
}

void
shader_set_uniform_v3i_array(uniform uniform, v3i *values, u32 amount) {
  glUniform3iv(uniform, amount, (s32 *)values);
}

void
shader_set_uniform_v4i_array(uniform uniform, v4i *values, u32 amount) {
  glUniform4iv(uniform, amount, (s32 *)values);
}

void
shader_set_uniform_v2u_array(uniform uniform, v2u *values, u32 amount) {
  glUniform2uiv(uniform, amount, (u32 *)values);
}

void
shader_set_uniform_v3u_array(uniform uniform, v3u *values, u32 amount) {
  glUniform3uiv(uniform, amount, (u32 *)values);
}

void
shader_set_uniform_v4u_array(uniform uniform, v4u *values, u32 amount) {
  glUniform4uiv(uniform, amount, (u32 *)values);
}

void
shader_set_uniform_m2_array(uniform uniform, m2 *values, u32 amount) {
  glUniformMatrix2fv(uniform, amount, GL_FALSE, (f32 *)values);
}

void
shader_set_uniform_m3_array(uniform uniform, m3 *values, u32 amount) {
  glUniformMatrix3fv(uniform, amount, GL_FALSE, (f32 *)values);
}

void
shader_set_uniform_m4_array(uniform uniform, m4 *values, u32 amount) {
  glUniformMatrix4fv(uniform, amount, GL_FALSE, (f32 *)values);
}

/*
 * *** Texture Atlas ***
 */

#define ATLAS_GET(FUNC, ATLAS, NAME) do { \
  (ATLAS) = hash_table_get(asset_manager.atlases, &(NAME));\
  if (!(ATLAS)) {\
    err("%s(): atlas '%.*s' isn't loaded.\n", #FUNC, (NAME).size, (NAME).buff);\
    exit(1);\
  }\
} while (0)

void
texture_atlas_setup(str name, u32 tile_width, u32 tile_height, u32 padding_x, u32 padding_y) {
  texture_atlas *atlas;
  ATLAS_GET(texture_atlas_setup, atlas, name);

  atlas->tile_size_px = V2F(tile_width, tile_height);
  atlas->tile_size    = v2f_mul(atlas->pixel_size, atlas->tile_size_px);
  atlas->tile_padding = v2f_mul(atlas->pixel_size, V2F(padding_x, padding_y));
}

texture_id
texture_atlas_get_id(str name) {
  texture_atlas *atlas;
  ATLAS_GET(texture_atlas_setup, atlas, name);
  return atlas->id;
}

/*
 * Sprite Font
 */

#define SPRITE_FONT_GET(FUNC, SPRITE_FONT, NAME) do { \
  (SPRITE_FONT) = hash_table_get(asset_manager.sprite_fonts, &(NAME));\
  if (!(SPRITE_FONT)) {\
    err("%s(): sprite font '%.*s' isn't loaded.\n", #FUNC, (NAME).size, (NAME).buff);\
    exit(1);\
  }\
} while (0)

void
sprite_font_setup(str name, u32 char_width, u32 char_height, u32 padding_x, u32 padding_y) {
  sprite_font *font;
  SPRITE_FONT_GET(sprite_font_setup, font, name);

  font->char_size_px        = V2F(char_width, char_height);
  font->char_size           = v2f_mul(font->pixel_size, font->char_size_px);
  font->char_sprite_padding = v2f_mul(font->pixel_size, V2F(padding_x, padding_y));
}

texture_id
sprite_font_get_id(str name) {
  sprite_font *font;
  SPRITE_FONT_GET(sprite_font_setup, font, name);
  return font->id;
}

/*
 * *** Texture Buffer
 */

typedef struct {
  texture_id id;
  u32 width;
  u32 height;
} texture_buff_header;

#define TEXTURE_BUFF_HEADER(BUFF) (((texture_buff_header *)BUFF) - 1)

pixel *
texture_buff_create(u32 width, u32 height, texture_buff_attributes *attribs) {
  texture_buff_header *header = malloc(sizeof (texture_buff_header) + (width * height * sizeof (pixel)));
  pixel *buff = (pixel *)(header + 1);
  header->width = width;
  header->height = height;
  glGenTextures(1, &header->id);
  glBindTexture(GL_TEXTURE_2D, header->id);
  if (attribs) {
    switch (attribs->filter_min) {
      case T2D_LINEAR:
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        break;
      case T2D_NEAREST:
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        break;
    }
    switch (attribs->filter_mag) {
      case T2D_LINEAR:
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
      case T2D_NEAREST:
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    }
  } else {
    glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, buff);
  glBindTexture(GL_TEXTURE_2D, 0);
  return buff;
}

void
texture_buff_destroy(pixel *buff) {
  texture_buff_header *header = TEXTURE_BUFF_HEADER(buff);
  glDeleteTextures(1, &header->id);
  free(header);
}

/*
 * *** Camera ***
 */

static void
camera_init(void) {
  camera.angle = 0;
  camera.position = V2F_0;
  camera.scale = V2F(1, 1);
  camera.proj = M3_ID;

  f32 right  = camera.position.x + camera.width  * 0.5f;
  f32 left   = camera.position.x - camera.width  * 0.5f;
  f32 top    = camera.position.y + camera.height * 0.5f;
  f32 bottom = camera.position.y - camera.height * 0.5f;

  camera.proj._00 = 2.0f / (right - left);
  camera.proj._11 = 2.0f / (top - bottom);

  camera.proj._02 = -((right + left) / (right - left));
  camera.proj._12 = -((top + bottom) / (top - bottom));
}

void
camera_set_position(v2f position) {
  camera.position = position;
}

v2f
camera_get_position(void) {
  return camera.position;
}

void
camera_set_scale(v2f scale) {
  camera.scale = scale;
}

v2f
camera_get_scale(void) {
  return camera.scale;
}

void
camera_set_angle(f32 angle) {
  camera.angle = angle;
}

f32
camera_get_angle(void) {
  return camera.angle;
}

/*
 * *** Rendering ***
 */

static void
renderer_init(void) {
  renderer.quads_amount = 0;
  renderer.vertices = malloc(sizeof (vertex) * renderer.vertices_capa);
  u32 *indices = malloc(sizeof (u32) * renderer.indices_capa);

  u32 j = 0;
  for (u32 i = 0; i < renderer.indices_capa; i += 6) {
    indices[i + 0] = j + 0;
    indices[i + 1] = j + 1;
    indices[i + 2] = j + 2;
    indices[i + 3] = j + 0;
    indices[i + 4] = j + 2;
    indices[i + 5] = j + 3;
    j += 4;
  }

  glGenVertexArrays(1, &renderer.vao);
  glGenBuffers(1, &renderer.vbo);
  glGenBuffers(1, &renderer.ibo);

  glBindVertexArray(renderer.vao);

  glBindBuffer(GL_ARRAY_BUFFER, renderer.vao);
  glBufferData(GL_ARRAY_BUFFER, sizeof (vertex) * renderer.vertices_capa, renderer.vertices, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer.ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (u32) * renderer.indices_capa, indices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof (vertex), (void *)offsetof(vertex, position));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof (vertex), (void *)offsetof(vertex, texcoord));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof (vertex), (void *)offsetof(vertex, blend));

  free(indices);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  renderer.requests = malloc(sizeof (vertex **) * renderer.layers_amount);
  for (u32 i = 0; i < renderer.layers_amount; i++) {
    renderer.requests[i] = malloc(sizeof (vertex *) * BATCH_SHADERS_AMOUNT);
    for (u32 j = 0; j < BATCH_SHADERS_AMOUNT; j++) {
      renderer.requests[i][j] = array_list_create(sizeof (quad));
    }
  }

  asset_load(ASSET_SHADER, DEFAULT_SHADER_QUAD);
  asset_load(ASSET_SHADER, DEFAULT_SHADER_TEXTURE);
  shader_use_camera(DEFAULT_SHADER_QUAD, true);
  shader_use_camera(DEFAULT_SHADER_TEXTURE, true);

  asset_load(ASSET_SPRITE_FONT, DEFAULT_SPRITE_FONT);

  assert(BATCH_SHADERS_AMOUNT == 4);
  renderer.batch.shaders[BATCH_SHADER_QUAD]    = DEFAULT_SHADER_QUAD;
  renderer.batch.shaders[BATCH_SHADER_ATLAS]   = DEFAULT_SHADER_ATLAS;
  renderer.batch.shaders[BATCH_SHADER_FONT]    = DEFAULT_SHADER_FONT;
  renderer.batch.shaders[BATCH_SHADER_TEXBUFF] = DEFAULT_SHADER_TEXBUFF;
  renderer.batch.atlas                         = STR_0;
  renderer.batch.font                          = DEFAULT_SPRITE_FONT;
  renderer.batch.texture_buff                = 0;

}

void
submit_batch(void) {
  texture_id atlas_id = 0;
  if (renderer.batch.atlas.size > 0) {
    texture_atlas *atlas;
    ATLAS_GET(submit_batch, atlas, renderer.batch.atlas);
    atlas_id = atlas->id;
  }

  texture_id font_id;
  if (renderer.batch.font.size > 0) {
    sprite_font *font;
    SPRITE_FONT_GET(submit_batch, font, renderer.batch.font);
    font_id = font->id;
  } else {
    err("submit_batch(): A batch font needs to be set.\n");
    exit(1);
  }

  texture_id texbuff_id = 0;
  if (renderer.batch.texture_buff) {
    texture_buff_header *header = TEXTURE_BUFF_HEADER(renderer.batch.texture_buff);
    glBindTexture(GL_TEXTURE_2D, header->id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
        header->width, header->height, GL_BGRA, GL_UNSIGNED_BYTE, renderer.batch.texture_buff);
    glBindTexture(GL_TEXTURE_2D, 0);
    texbuff_id = header->id;
  }


  m3 view = M3_ID;
  m3 transform;

  /* camera scale matrix */
  transform = M3_ID;
  transform._00 = camera.scale.x;
  transform._11 = camera.scale.y;
  view = m3_mul(view, transform);

  /* camera translation matrix */
  transform = M3_ID;
  transform._02 = -camera.position.x;
  transform._12 = -camera.position.y;
  view = m3_mul(view, transform);

  /* camera rotation matrix */
  transform = M3_ID;
  transform._00 = +cosf(camera.angle);
  transform._01 = -sinf(camera.angle);
  transform._10 = +sinf(camera.angle);
  transform._11 = +cosf(camera.angle);
  view = m3_mul(view, transform);

  /* camera matrix */
  m3 camera_matrix = m3_mul(camera.proj, view);

  /* submit batches */
  for (batch_shader_type k = 0; k < BATCH_SHADERS_AMOUNT; k++) {
    u32 vertices_amount = 0;
    u32 indices_amount  = 0;
    shader_data *shader;
    SHADER_GET(submit_batch, shader, renderer.batch.shaders[k]);
    glUseProgram(shader->id);
    if (shader->use_camera_projection) {
      shader_set_uniform_m3(shader->u_camera, camera_matrix);
    }
    switch (k) {
      case BATCH_SHADER_ATLAS:   glBindTexture(GL_TEXTURE_2D, atlas_id);    break;
      case BATCH_SHADER_FONT:    glBindTexture(GL_TEXTURE_2D, font_id);     break;
      case BATCH_SHADER_TEXBUFF: glBindTexture(GL_TEXTURE_2D, texbuff_id);  break;
      case BATCH_SHADER_QUAD:                                               break;
      case BATCH_SHADERS_AMOUNT:                                            break;
    };
    for (u32 i = 0; i < renderer.layers_amount; i++) {
      for (u32 j = 0; j < array_list_size(renderer.requests[i][k]); j++) {
        renderer.vertices[vertices_amount++] = (vertex) {
          .position = renderer.requests[i][k][j][0].position,
          .texcoord = renderer.requests[i][k][j][0].texcoord,
          .blend    = renderer.requests[i][k][j][0].blend
        };
        renderer.vertices[vertices_amount++] = (vertex) {
          .position = renderer.requests[i][k][j][1].position,
          .texcoord = renderer.requests[i][k][j][1].texcoord,
          .blend    = renderer.requests[i][k][j][1].blend
        };
        renderer.vertices[vertices_amount++] = (vertex) {
          .position = renderer.requests[i][k][j][2].position,
          .texcoord = renderer.requests[i][k][j][2].texcoord,
          .blend    = renderer.requests[i][k][j][2].blend
        };
        renderer.vertices[vertices_amount++] = (vertex) {
          .position = renderer.requests[i][k][j][3].position,
          .texcoord = renderer.requests[i][k][j][3].texcoord,
          .blend    = renderer.requests[i][k][j][3].blend
        };
        indices_amount += 6;
      }
      array_list_clear(renderer.requests[i][k]);
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_amount * sizeof (vertex), renderer.vertices);
    glDrawElements(GL_TRIANGLES, indices_amount, GL_UNSIGNED_INT, 0);
  }

  renderer.quads_amount = 0;
}

void
clear_screen(v4f color) {
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(color.x, color.y, color.z, color.w);
}

void
draw_quad(v2f position, v2f size, v4f blend, u32 layer) {
  if (layer >= renderer.layers_amount) {
    err("draw_quad(): out of bounds layer: %u.\n", layer);
    exit(1);
  }

  if (renderer.quads_amount * 4 >= renderer.vertices_capa) {
    submit_batch();
  }
  quad quad;
  v2f hsize = v2f_mul(size, V2F(0.5f, 0.5f));
  quad[0].position = V2F(position.x - hsize.x, position.y - hsize.y);
  quad[1].position = V2F(position.x + hsize.x, position.y - hsize.y);
  quad[2].position = V2F(position.x + hsize.x, position.y + hsize.y);
  quad[3].position = V2F(position.x - hsize.x, position.y + hsize.y);

  quad[0].blend = blend;
  quad[1].blend = blend;
  quad[2].blend = blend;
  quad[3].blend = blend;

  renderer.requests[layer][BATCH_SHADER_QUAD] = array_list_grow(renderer.requests[layer][BATCH_SHADER_QUAD], 1);
  for (u32 i = 0; i < 4; i++) {
    renderer.requests [layer] [BATCH_SHADER_QUAD] [array_list_size(renderer.requests[layer][BATCH_SHADER_QUAD]) - 1][i] = quad[i];
  }

  renderer.quads_amount++;
}

void
draw_tile(v2u tile, v2f position, v2f scale, v4f blend, u32 layer) {
  if (layer >= renderer.layers_amount) {
    err("draw_tile(): out of bounds layer: %u.\n", layer);
    exit(1);
  }

  if (renderer.batch.atlas.size == 0) {
    err("draw_tile(): trying to draw a tile without using an atlas.\n");
    exit(1);
  }

  texture_atlas *atlas;
  ATLAS_GET(draw_tile, atlas, renderer.batch.atlas);

  if (renderer.quads_amount * 4 >= renderer.vertices_capa) {
    submit_batch();
  }

  v2f tile_pos = v2f_add(
    v2f_mul(atlas->tile_size,    V2F(tile.x, tile.y)),
    v2f_mul(atlas->tile_padding, V2F(tile.x, tile.y))
  );

  v2f hsize = 
    v2f_mul(v2f_mul(atlas->tile_size_px, scale), V2F(0.5f, 0.5f));

  quad quad;
  quad[0].position = V2F(position.x - hsize.x, position.y - hsize.y);
  quad[1].position = V2F(position.x + hsize.x, position.y - hsize.y);
  quad[2].position = V2F(position.x + hsize.x, position.y + hsize.y);
  quad[3].position = V2F(position.x - hsize.x, position.y + hsize.y);

  quad[0].texcoord = v2f_add(tile_pos, V2F(0,                  atlas->tile_size.y));
  quad[1].texcoord = v2f_add(tile_pos, V2F(atlas->tile_size.x, atlas->tile_size.y));
  quad[2].texcoord = v2f_add(tile_pos, V2F(atlas->tile_size.x, 0                 ));
  quad[3].texcoord = v2f_add(tile_pos, V2F(0,                  0                 ));

  quad[0].blend = blend;
  quad[1].blend = blend;
  quad[2].blend = blend;
  quad[3].blend = blend;

  renderer.requests[layer][BATCH_SHADER_ATLAS] =
    array_list_grow(renderer.requests[layer][BATCH_SHADER_ATLAS], 1);
  for (u32 i = 0; i < 4; i++) {
    renderer.requests
      [layer]
      [BATCH_SHADER_ATLAS]
      [array_list_size(
          renderer.requests
          [layer]
          [BATCH_SHADER_ATLAS]) - 1]
      [i] = quad[i];
  }

  renderer.quads_amount++;
}

#define DRAW_TEXT_CAP 512
#define UNK_CHAR ('~'+1)
void
draw_text(v2f position, v2f scale, v4f blend, u32 layer, str fmt, ...) {
  if (layer >= renderer.layers_amount) {
    err("draw_text(): out of bounds layer: %u.\n", layer);
    exit(1);
  }

  if (renderer.batch.font.size == 0) {
    err("draw_text(): trying to draw text without using a font.\n");
    exit(1);
  }

  sprite_font *font;
  SPRITE_FONT_GET(draw_text, font, renderer.batch.font);

  va_list args;
  va_start(args, fmt);
  u8 chars[DRAW_TEXT_CAP];
  vsnprintf((cstr)chars, DRAW_TEXT_CAP, fmt.buff, args);
  va_end(args);

  v2f text_cursor = V2F_0;
  for (u32 i = 0; i < DRAW_TEXT_CAP; i++) {
    if (chars[i] == '\0') return;
    if (renderer.quads_amount * 4 >= renderer.vertices_capa) {
      submit_batch();
    }

    u8 c = chars[i];
    if ((c < ' ' || c > '~') && c != '\n') c = UNK_CHAR;
    if (c == ' ') {
      text_cursor.x++;
      continue;
    }
    if (c == '\n') {
      text_cursor.y--;
      text_cursor.x = 0;
      continue;
    }
    v2f char_siz = v2f_mul(scale, font->char_size_px);
    v2f char_font_pos = V2F((c - '!') * font->char_size.x + font->char_sprite_padding.x, 0);
    v2f char_pad = v2f_mul(scale, font->char_padding);
    v2f char_pos = v2f_add(position, v2f_mul(text_cursor, v2f_add(char_siz, char_pad)));
    v2f hchar_siz = v2f_mul(char_siz, V2F(0.5f, 0.5f));
    quad quad;
    quad[0].position = V2F(char_pos.x - hchar_siz.x, char_pos.y - hchar_siz.y);
    quad[1].position = V2F(char_pos.x + hchar_siz.x, char_pos.y - hchar_siz.y);
    quad[2].position = V2F(char_pos.x + hchar_siz.x, char_pos.y + hchar_siz.y);
    quad[3].position = V2F(char_pos.x - hchar_siz.x, char_pos.y + hchar_siz.y);

    quad[0].texcoord = v2f_add(char_font_pos, V2F(0,                 font->char_size.y));
    quad[1].texcoord = v2f_add(char_font_pos, V2F(font->char_size.x, font->char_size.y));
    quad[2].texcoord = v2f_add(char_font_pos, V2F(font->char_size.x, 0                ));
    quad[3].texcoord = v2f_add(char_font_pos, V2F(0,                 0                ));

    quad[0].blend = blend;
    quad[1].blend = blend;
    quad[2].blend = blend;
    quad[3].blend = blend;

    renderer.requests[layer][BATCH_SHADER_FONT] =
      array_list_grow(renderer.requests[layer][BATCH_SHADER_FONT], 1);
    for (u32 i = 0; i < 4; i++) {
      renderer.requests
        [layer]
        [BATCH_SHADER_FONT]
        [array_list_size(
            renderer.requests
            [layer]
            [BATCH_SHADER_FONT]
        ) - 1]
        [i] = quad[i];
    }

    renderer.quads_amount++;
    text_cursor.x++;
  }

}
#undef DRAW_TEXT_CAP
#undef UNK_CHAR

void
draw_texture_buff(v2f position, v2f size, v4f blend, u32 layer, v2f *parts) {
  if (layer >= renderer.layers_amount) {
    err("draw_texture_buff(): out of bounds layer: %u.\n", layer);
    exit(1);
  }

  if (!renderer.batch.texture_buff) {
    err("draw_texture_buff(): no texture buffer bounded to the current batch.\n");
    exit(1);
  }

  if (renderer.quads_amount * 4 >= renderer.vertices_capa) {
    submit_batch();
  }

  quad quad;
  v2f hsize = v2f_mul(size, V2F(0.5f, 0.5f));
  quad[0].position = V2F(position.x - hsize.x, position.y - hsize.y);
  quad[1].position = V2F(position.x + hsize.x, position.y - hsize.y);
  quad[2].position = V2F(position.x + hsize.x, position.y + hsize.y);
  quad[3].position = V2F(position.x - hsize.x, position.y + hsize.y);

  if (parts) {
    quad[0].texcoord = parts[0];
    quad[1].texcoord = parts[1];
    quad[2].texcoord = parts[2];
    quad[3].texcoord = parts[3];
  } else {
    quad[0].texcoord = V2F(0, 1);
    quad[1].texcoord = V2F(1, 1);
    quad[2].texcoord = V2F(1, 0);
    quad[3].texcoord = V2F(0, 0);
  }

  quad[0].blend = blend;
  quad[1].blend = blend;
  quad[2].blend = blend;
  quad[3].blend = blend;

  renderer.requests[layer][BATCH_SHADER_TEXBUFF] =
    array_list_grow(renderer.requests[layer][BATCH_SHADER_TEXBUFF], 1);
  for (u32 i = 0; i < 4; i++) {
    renderer.requests
      [layer]
      [BATCH_SHADER_TEXBUFF]
      [array_list_size(
          renderer.requests
          [layer]
          [BATCH_SHADER_TEXBUFF]) - 1]
      [i] = quad[i];
  }

  renderer.quads_amount++;
}

/*
 * Input
 */

static void
key_callback(GLFWwindow *window, s32 key, s32 scancode, s32 action, s32 mods) {
  if (action == GLFW_REPEAT || key == GLFW_KEY_UNKNOWN) return;
  (void)window; (void)scancode; (void)mods;
  b8 pressed = action == GLFW_PRESS;
  if (key < 0x80) {
    input.keyboard.keys_cur[key] = pressed;
    return;
  }
  if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F12) {
    input.keyboard.keys_cur[key - GLFW_KEY_F1 + KEY_F1] = pressed;
    return;
  }
  switch (key) {
    case GLFW_KEY_RIGHT:            input.keyboard.keys_cur[KEY_RIGHT]      = pressed; break;
    case GLFW_KEY_LEFT:             input.keyboard.keys_cur[KEY_LEFT]       = pressed; break;
    case GLFW_KEY_UP:               input.keyboard.keys_cur[KEY_UP]         = pressed; break;
    case GLFW_KEY_DOWN:             input.keyboard.keys_cur[KEY_DOWN]       = pressed; break;
    case GLFW_KEY_LEFT_SHIFT:       input.keyboard.keys_cur[KEY_LSHIFT]     = pressed; break;
    case GLFW_KEY_LEFT_CONTROL:     input.keyboard.keys_cur[KEY_LCONTROL]   = pressed; break;
    case GLFW_KEY_LEFT_ALT:         input.keyboard.keys_cur[KEY_LALT]       = pressed; break;
    case GLFW_KEY_RIGHT_SHIFT:      input.keyboard.keys_cur[KEY_RSHIFT]     = pressed; break;
    case GLFW_KEY_RIGHT_CONTROL:    input.keyboard.keys_cur[KEY_RCONTROL]   = pressed; break;
    case GLFW_KEY_RIGHT_ALT:        input.keyboard.keys_cur[KEY_RALT]       = pressed; break;
    case GLFW_KEY_ESCAPE:           input.keyboard.keys_cur[KEY_ESCAPE]     = pressed; break;
    case GLFW_KEY_BACKSPACE:        input.keyboard.keys_cur[KEY_BACKSPACE]  = pressed; break;
    case GLFW_KEY_TAB:              input.keyboard.keys_cur[KEY_TAB]        = pressed; break;
    case GLFW_KEY_ENTER:            input.keyboard.keys_cur[KEY_RETURN]     = pressed; break;
  }
}

static void
mouse_button_callback(GLFWwindow *window, s32 button, s32 action, s32 mods) {
  (void)window; (void)mods;
  if (button > 2) return;
  input.mouse.buttons_cur[button] = action == GLFW_PRESS;
}

static void
mouse_move_callback(GLFWwindow *window, f64 x, f64 y) {
  (void)window;
  input.mouse.screen_position.x = x;
  input.mouse.screen_position.y = y;
}

static void
scroll_callback(GLFWwindow *window, f64 xoffset, f64 yoffset) {
  (void)window;
  input.mouse.scroll.x = xoffset;
  input.mouse.scroll.y = yoffset;
}

b8 
key_press(input_index key) {
  return input.keyboard.keys_cur[key];
}

b8 
key_click(input_index key) {
  return input.keyboard.keys_cur[key] && !input.keyboard.keys_prv[key];
}

b8 
key_click_tick(input_index key) {
  return input.keyboard.keys_cur[key] && !input.keyboard.keys_tick_prv[key];
}


b8 
button_press(input_index button) {
  return input.mouse.buttons_cur[button];
}

b8 
button_click(input_index button) {
  return input.mouse.buttons_cur[button] && !input.mouse.buttons_prv[button];
}

b8 
button_click_tick(input_index button) {
  return input.mouse.buttons_cur[button] && !input.mouse.buttons_tick_prv[button];
}

v2f
mouse_get_position(void) {
  return input.mouse.position;
}

v2f
mouse_get_screen_position(void) {
  return input.mouse.screen_position;
}

v2f
mouse_get_scroll(void) {
  return input.mouse.scroll;
}

/*
 * *** Window and Context things ***
 * */

static GLFWwindow *window;

static f32 tick_acc;
static f32 ticks_per_second;

extern void __conf(blib_config *config);
extern void __init(void);
extern void __loop(f32 dt);
extern void __tick(f32 dt);
extern void __draw(batch *batch);
extern void __quit(void);

static void
window_create(void) {
  blib_config config;
  config.window_title       = "Blib App";
  config.window_width       = 640;
  config.window_height      = 480;
  config.window_center      = true;
  config.window_resizable   = false;
  config.camera_width       = config.window_width;
  config.camera_height      = config.window_height;
  config.quads_capacity     = 10000;
  config.layers_amount      = 5;
  config.ticks_per_second   = 60;
  __conf(&config);
  renderer.vertices_capa = config.quads_capacity * 4;
  renderer.indices_capa  = config.quads_capacity * 6;
  renderer.layers_amount = config.layers_amount;
  camera.width           = config.camera_width;
  camera.height          = config.camera_height;
  ticks_per_second       = 1.0f / config.ticks_per_second;

  if (!glfwInit()) {
    ccstr desc;
    glfwGetError(&desc);
    err("GLFW: %s\n", desc);
    exit(1);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_RESIZABLE, config.window_resizable ? GLFW_TRUE : GLFW_FALSE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if macintosh || Macintosh || (__APPLE__ && __MACH__)
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif
  if (config.window_width  <= 0) config.window_width  = 640;
  if (config.window_height <= 0) config.window_height = 480;
  window = glfwCreateWindow(config.window_width, config.window_height, config.window_title, 0, 0);
  if (!window) {
    ccstr desc;
    glfwGetError(&desc);
    err("GLFW: %s\n", desc);
    glfwTerminate();
    exit(1);
  }
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  if (config.window_center) {
    const GLFWvidmode *vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(window, 
        config.window_width  >= vidmode->width  ? 0 : (vidmode->width  >> 1) - (config.window_width  >> 1),
        config.window_height >= vidmode->height ? 0 : (vidmode->height >> 1) - (config.window_height >> 1));
  }
  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, mouse_move_callback);
  glfwSetScrollCallback(window, scroll_callback);

  glfwSwapInterval(0);
}

static void
window_destroy(void) {
  glfwTerminate();
}

void
close_window(void) {
  glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void
enable_vsync(b8 enable) {
  glfwSwapInterval(enable);
}

s32
main(void) {
  srand(time(0));

  window_create();

  entity_system_init();
  asset_manager_init();
  renderer_init();
  camera_init();

  __init();
  f32 prev_time = glfwGetTime();
  while (!glfwWindowShouldClose(window)) {
    f32 dt = glfwGetTime() - prev_time;
    prev_time = glfwGetTime();
    __loop(dt);
    __draw(&renderer.batch);
    tick_acc += dt;
    if (tick_acc >= ticks_per_second) {
      tick_acc = 0;
      __tick(dt);
      memcpy(input.keyboard.keys_tick_prv, input.keyboard.keys_cur, sizeof (b8) * KEY_CAP);
      memcpy(input.mouse.buttons_tick_prv, input.mouse.buttons_cur, sizeof (b8) * BTN_CAP);
    }
    memcpy(input.keyboard.keys_prv, input.keyboard.keys_cur, sizeof (b8) * KEY_CAP);
    memcpy(input.mouse.buttons_prv, input.mouse.buttons_cur, sizeof (b8) * BTN_CAP);


    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  __quit();

  window_destroy();

  return 0;
}

