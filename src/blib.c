#include "blib.h"

#include <GLFW/glfw3.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux
#include <uuid/uuid.h>
#endif

#define inf(...) fprintf(stderr, "Info:  " __VA_ARGS__)
#define wrn(...) fprintf(stderr, "Warn:  " __VA_ARGS__)
#define err(...) fprintf(stderr, "Error: " __VA_ARGS__)

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
  str.capa = str.size + !str.size; /* if str.size == 0 then str.capa = 1 else str.capa = str.size */
  str.buff = malloc(sizeof (char) * str.capa);
  if (str.size) {
    memcpy(str.buff, src.buff, sizeof (char) * src.size);
  }
  return str;
}

void
string_reserve(str *str, u32 amount) {
  if (!str->capa) {
    wrn("string_reserve(): `dest` must have been created by `string_create()`\n");
    return;
  }
  str->capa += amount;
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
    return;
  }
  if (dest->capa < src.size) {
    dest->capa += src.size;
    dest->buff = realloc(dest->buff, sizeof (char) * dest->capa);
  }
  memcpy(dest->buff, src.buff, sizeof (char) * src.size);
  dest->size = src.size;
}

void
string_concat(str *dest, str src) {
  if (!dest->capa) {
    wrn("string_copy(): `dest` must have been created by `string_create()`\n");
    return;
  }
  if (!src.size) return;
  if (dest->size + src.size > src.capa) {
    dest->capa += src.size;
    dest->buff = realloc(dest->buff, sizeof (char) * dest->capa);
  }
  memcpy(dest->buff + dest->size, src.buff, sizeof (char) * src.size);
  dest->size += src.size;
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
  if (dest->size + src.size > dest->capa) {
    dest->capa += src.size;
    dest->buff = realloc(dest->buff, sizeof (char) * dest->capa);
  }
  memmove(dest->buff + index + src.size, dest->buff + index, dest->size - index);
  memcpy(dest->buff + index, src.buff, src.size);
  dest->size += src.size;
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
    wrn("string_reverse(): `dest` must have been created by `string_create()`\n");
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
      break;
    case HT_U64:
      return hash_u64(*(u64 *)key);
      break;
    case HT_U128:
      return hash_u128(*(u128 *)key);
      break;
    case HT_AMOUNT:
      err("hash(): unreachable\n");
      exit(1);
      break;
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
          return;
        }
        break;
      case HT_U64:
        if (ht->keys.u64[index] == *(u64 *)key) {
          ht->free[index] = true;
          return;
        }
        break;
      case HT_U128:
        if (ht->keys.u128[index].u64[0] == ((u128 *)key)->u64[0] &&
            ht->keys.u128[index].u64[1] == ((u128 *)key)->u64[1]) {
          ht->free[index] = true;
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

typedef struct {
  void *list;
  u32   type;
} entity_component;

typedef struct {
  str *component_names;
  hash_table *components;
  hash_table *indexes;
  u32 amount;
  u32 name_index;
  u32 *destroy_stack;
} entity_type;

typedef struct {
  str *entity_type_names;
  hash_table *entities;
  entity_type *new_type;
} entity_system;

static struct {
  entity_system entity_system;
  blib_config config;
  void *scenes;
  u32 scenes_amount;
} blib_context;


/*
 * *** Entity System ***
 */ 

void
entity_type_begin(str name) {
  if (blib_context.entity_system.new_type) {
    wrn("entity_type_begin(): trying to create two entity types at the same time. forgot entity_type_end()?\n");
    return;
  }
  if (!name.size) {
    wrn("entity_type_begin(): entity type name can't be empty\n");
    return;
  }
  if (hash_table_get(blib_context.entity_system.entities, &name)) {
    wrn("entity_type_begin(): entity type '%.*s' already exists\n", name.size, name.buff);
    return;
  }
  blib_context.entity_system.new_type = hash_table_add(blib_context.entity_system.entities, &name);
  blib_context.entity_system.new_type->component_names = array_list_create(sizeof (str));
  blib_context.entity_system.new_type->components      = hash_table_create(sizeof (entity_component), HT_STR);
  blib_context.entity_system.new_type->indexes         = hash_table_create(sizeof (u32), HT_U128);
  blib_context.entity_system.new_type->name_index      = array_list_size(blib_context.entity_system.entity_type_names);
  blib_context.entity_system.new_type->amount          = 0;
  array_list_push(blib_context.entity_system.entity_type_names, string_create(name));
}

void
entity_type_add_component(str name, u32 size) {
  if (!blib_context.entity_system.new_type) {
    wrn("entity_type_add_component(): trying to create add component without initiating a new entity type. forgot entity_type_begin()?\n");
    return;
  }
  if (!name.size) {
    wrn("entity_type_add_component(): entity component name can't be empty\n");
    return;
  }
  if (hash_table_get(blib_context.entity_system.new_type->components, &name)) {
    str entity_type_name;
    hash_table_value_key(blib_context.entity_system.entities, blib_context.entity_system.new_type, &entity_type_name);
    wrn("entity_type_add_component(): component '%.*s' already exists on entity '%.*s'\n",
        name.size, name.buff, entity_type_name.size, entity_type_name.buff);
    return;
  }
  entity_component *component = hash_table_add(blib_context.entity_system.new_type->components, &name);
  component->list = array_list_create(size);
  component->type = size;
  array_list_push(blib_context.entity_system.new_type->component_names, string_create(name));
}

void
entity_type_end(void) {
  blib_context.entity_system.new_type = 0;
}

void *
entity_type_get_components(str type_name, str comp_name) {
  entity_type *type = hash_table_get(blib_context.entity_system.entities, &type_name);
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

entity
entity_create(str type_name) {
  entity_type *type = hash_table_get(blib_context.entity_system.entities, &type_name);
  if (!type) {
    wrn("entity_create(): type '%.*s' doesn't exists\n", type_name.size, type_name.buff);
    return (entity){0};
  }
  entity e;

  e.type = type->name_index;

#ifdef __linux
  uuid_generate((u8 *)&e.id);
#endif

  u32 *index = hash_table_add(type->indexes, &e.id);
  if (!index) {
    err("entity_create(): unreachable\n");
    return (entity) { 0 };
  }
  *index = type->amount++;
  for (u32 i = 0; i < array_list_size(type->component_names); i++) {
    entity_component *component = hash_table_get(type->components, &type->component_names[i]);
    component->list = array_list_grow(component->list, 1);
  }
  return e;
}

void *
entity_get_component(entity e, str comp_name) {
  entity_type *type = hash_table_get(blib_context.entity_system.entities, &blib_context.entity_system.entity_type_names[e.type]);
  if (!type) {
    wrn("entity_get_component(): entity with invalid type\n");
    return 0;
  }
  entity_component *component = hash_table_get(type->components, &comp_name);
  if (!component) {
    wrn("entity_get_component(): unexisting component '%.*s'\n", comp_name.size, comp_name.buff);
    return 0;
  }
  u32 *index = hash_table_get(type->indexes, &e.id);
  if (!index) {
    wrn("entity_get_component(): entity doesn't exists\n");
    return 0;
  }
  return (u8 *)component->list + (*index) * component->type;
}

void
entity_destroy(entity e) {
  entity_type *type = hash_table_get(blib_context.entity_system.entities, &blib_context.entity_system.entity_type_names[e.type]);
  if (!type) {
    wrn("entity_destroy(): entity with invalid type\n");
    return;
  }
  u32 *index = hash_table_get(type->indexes, &e.id);
  if (!index) {
    wrn("entity_get_component(): entity doesn't exists\n");
    return;
  }
  array_list_push(type->destroy_stack, *index);
}

/*
 *
 * *** Window and Context things ***
 *
 * */

static GLFWwindow *window;

extern void __config(blib_config *config);
extern void __scenes(void *scenes, u32 scenes_amount);

s32
main(void) {
  blib_context.config.title                    = "Blib App";
  blib_context.config.width                    = 640;
  blib_context.config.height                   = 480;
  blib_context.config.center                   = true;
  blib_context.config.resizable                = false;
  blib_context.config.scenes_amount            = 1;
  blib_context.config.compatibility_profile    = false;
  blib_context.config.opengl_major             = 3;
  blib_context.config.opengl_minor             = 3;
  blib_context.scenes                          = 0;
  blib_context.entity_system.new_type          = 0;
  blib_context.entity_system.entities          = hash_table_create(sizeof (entity_type), HT_STR);
  blib_context.entity_system.entity_type_names = array_list_create(sizeof (str));
  __config(&blib_context.config);
  blib_context.scenes_amount = blib_context.config.scenes_amount;
  if (!blib_context.scenes_amount) {
    err("The application needs to have at least one scene\n");
    exit(1);
  }
  __scenes(blib_context.scenes, blib_context.scenes_amount);
  if (!glfwInit()) {
    ccstr desc;
    glfwGetError(&desc);
    err("GLFW: %s\n", desc);
    exit(1);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, blib_context.config.opengl_major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, blib_context.config.opengl_minor);
  glfwWindowHint(GLFW_RESIZABLE, blib_context.config.resizable ? GLFW_TRUE : GLFW_FALSE);
  glfwWindowHint(GLFW_OPENGL_PROFILE,
      blib_context.config.compatibility_profile ? GLFW_OPENGL_COMPAT_PROFILE : GLFW_OPENGL_CORE_PROFILE);
#if macintosh || Macintosh || (__APPLE__ && __MACH__)
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif
  if (blib_context.config.width  <= 0) blib_context.config.width  = 640;
  if (blib_context.config.height <= 0) blib_context.config.height = 480;
  window = glfwCreateWindow(blib_context.config.width, blib_context.config.height, blib_context.config.title, 0, 0);
  if (!window) {
    ccstr desc;
    glfwGetError(&desc);
    err("GLFW: %s\n", desc);
    glfwTerminate();
    exit(1);
  }
  glfwMakeContextCurrent(window);
  if (blib_context.config.center) {
    const GLFWvidmode *vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(window, 
        blib_context.config.width  >= vidmode->width  ? 0 : (vidmode->width  >> 1) - (blib_context.config.width  >> 1),
        blib_context.config.height >= vidmode->height ? 0 : (vidmode->height >> 1) - (blib_context.config.height >> 1));
  }

  while (!glfwWindowShouldClose(window)) {
    for (u32 i = 0; i < array_list_size(blib_context.entity_system.entity_type_names); i++) {
      entity_type *type = hash_table_get(blib_context.entity_system.entities, &blib_context.entity_system.entity_type_names[i]);
      for (u32 j = 0; j < array_list_size(type->destroy_stack); j++) {
        for (u32 k = 0; k < array_list_size(type->component_names); k++) {
          entity_component *component = hash_table_get(type->components, &type->component_names[k]);
          array_list_remove(component->list, type->destroy_stack[j], 0);
        }
      }
    }
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}

