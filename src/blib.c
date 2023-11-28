#include "blib.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

static struct {
  hash_table *shaders;
  str path;
} asset_manager;

static void
asset_manager_init(void) {
  asset_manager.shaders = hash_table_create(sizeof (shader_id), HT_STR);
  asset_manager.path = string_create(STR_0);
  string_reserve(&asset_manager.path, 1024);
}


enum {
  SHADER_CREATE_RESULT_SUCCESS = 0,
  SHADER_CREATE_RESULT_INVALID_TYPE,
  SHADER_CREATE_RESULT_INVALID_PATH,
};

typedef struct {
  u32 sh;
  u32 exit;
} shader_create_result;

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

void
asset_load(asset_type type, str name) {
  switch (type) {
    case ASSET_SHADER:
    {
      shader_id *shader = hash_table_add(asset_manager.shaders, &name);
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

      *shader = glCreateProgram();
      glAttachShader(*shader, vertex);
      glAttachShader(*shader, fragment);
      glLinkProgram(*shader);

      s32 status;
      glGetProgramiv(*shader, GL_LINK_STATUS, &status);
      if (!status) {
        s32 log_size;
        glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &log_size);
        cstr log = malloc(log_size);
        glGetProgramInfoLog(*shader, log_size, 0, log);
        err("OpenGL: '%.*s' linker: %.*s\n", name.size,  name.buff, log_size, log);
        exit(1);
      }
      glDeleteShader(vertex);
      glDeleteShader(fragment);
    } break;
  }
}

void
asset_unload(asset_type type, str name) {
  switch (type) {
    case ASSET_SHADER:
    {
      shader_id *shader = hash_table_get(asset_manager.shaders, &name);
      if (!shader) {
        wrn("asset_unload(): already unloaded shader '%.*s'.\n", name.size, name.buff);
        return;
      }
      glDeleteProgram(*shader);
      hash_table_del(asset_manager.shaders, &name);
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

uniform
shader_get_uniform(str shader_name, str uniform_name) {
  shader_id *shader;
  SHADER_GET(shader_get_uniform, shader, shader_name);
  uniform uniform = glGetUniformLocation(*shader, uniform_name.buff);
  if (uniform == -1) {
    err("shader_get_uniform(): uniform '%.*s' of shader '%.*s' doesn't exists\n", uniform_name.size, uniform_name.buff, shader_name.size, shader_name.buff);
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
shader_set_uniform_v2(uniform uniform, v2 value) {
  glUniform2f(uniform, value.x, value.y);
}

void
shader_set_uniform_v3(uniform uniform, v3 value) {
  glUniform3f(uniform, value.x, value.y, value.z);
}

void
shader_set_uniform_v4(uniform uniform, v4 value) {
  glUniform4f(uniform, value.x, value.y, value.z, value.w);
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
shader_set_uniform_v2_array(uniform uniform, v2 *values, u32 amount) {
  glUniform2fv(uniform, amount, (f32 *)values);
}

void
shader_set_uniform_v3_array(uniform uniform, v3 *values, u32 amount) {
  glUniform3fv(uniform, amount, (f32 *)values);
}

void
shader_set_uniform_v4_array(uniform uniform, v4 *values, u32 amount) {
  glUniform4fv(uniform, amount, (f32 *)values);
}

/*
 * *** Texture 2D
 */

#if 0
typedef struct {
  u32 id;
  u32 width;
  u32 height;
  pixel_type type;
} texture_2d_header;
static u32 current_setted_texture_id = 0;

#define TEXTURE_2D_HEADER(TEX) (((texture_2d_header *)TEX) - 1)

texture_2d *
texture_2d_create(u32 width, u32 height, pixel_type pixel_type, texture_2d_attributes *attribs) {
  texture_2d_header *header = malloc(sizeof (texture_2d_header) + (width * height * sizeof (pixel)));
  texture_2d *tex = (texture_2d *)(header + 1);
  memset(tex, 0, width * height);
  header->type = pixel_type;
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
  switch (pixel_type) {
    case PIXEL_RGBA:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);
      break;
    case PIXEL_BGRA:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, tex);
      inf("w: %u, h: %u\n", width, height);
      break;
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  return tex;
}

void
texture_2d_set(texture_2d *tex) {
  current_setted_texture_id = !tex ? 0 : TEXTURE_2D_HEADER(tex)->id;
  glBindTexture(GL_TEXTURE_2D, current_setted_texture_id);
}

void
texture_2d_update(texture_2d *tex) {
  texture_2d_header *header = TEXTURE_2D_HEADER(tex);
  glBindTexture(GL_TEXTURE_2D, header->id);
  switch (header->type) {
    case PIXEL_RGBA:
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, header->width, header->height, GL_RGBA, GL_UNSIGNED_BYTE, tex);
      inf("w: %u, h: %u\n", header->width, header->height);
      break;
    case PIXEL_BGRA:
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, header->width, header->height, GL_BGRA, GL_UNSIGNED_BYTE, tex);
      break;
  }
  glBindTexture(GL_TEXTURE_2D, current_setted_texture_id);
}

void
texture_2d_destroy(texture_2d *tex) {
  texture_2d_header *header = TEXTURE_2D_HEADER(tex);
  glDeleteTextures(1, &header->id);
  free(header);
}
#endif

/*
 * *** Rendering ***
 */

typedef struct {
  v2 position;
  v2 texcoord;
  v4 blend;
} vertex;

typedef vertex quad[4];

static struct {
  u32 quads_amount;
  u32 vertices_capa;
  u32 indices_capa;
  vertex *vertices;

  u32 layers_amount;
  quad ***requests;

  str batch_shaders[BATCH_SHADERS_AMOUNT];

  u32 vao;
  u32 vbo;
  u32 ibo;
} renderer;

static void
renderer_init(void) {
  asset_load(ASSET_SHADER, STR("quad"));
  asset_load(ASSET_SHADER, STR("texture"));

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

  renderer.requests = malloc(sizeof (vertex **) * renderer.layers_amount);
  for (u32 i = 0; i < renderer.layers_amount; i++) {
    renderer.requests[i] = malloc(sizeof (vertex *) * BATCH_SHADERS_AMOUNT);
    for (u32 j = 0; j < BATCH_SHADERS_AMOUNT; j++) {
      renderer.requests[i][j] = array_list_create(sizeof (quad));
    }
  }

  assert(BATCH_SHADERS_AMOUNT == 2);
  renderer.batch_shaders[BATCH_SHADER_QUAD] = DEFAULT_SHADER_QUAD;
  renderer.batch_shaders[BATCH_SHADER_TEXTURE] = DEFAULT_SHADER_TEXTURE;
}

void
submit_batch(void) {
  for (u32 k = 0; k < BATCH_SHADERS_AMOUNT; k++) {
    u32 vertices_amount = 0;
    u32 indices_amount  = 0;
    shader_id *shader;
    SHADER_GET(submit_batch, shader, renderer.batch_shaders[k]);
    glUseProgram(*shader);
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
clear_screen(v4 color) {
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(color.x, color.y, color.z, color.w);
}

void
draw_quad(v2 position, v2 size, v4 blend, u32 layer) {
  if (layer >= renderer.layers_amount) {
    err("draw_quad(): out of bounds layer: %u.\n", layer);
    exit(1);
  }

  if (renderer.quads_amount * 4 >= renderer.vertices_capa) {
    submit_batch();
  }
  quad quad;
  quad[0].position = (v2) { position.x,          position.y          };
  quad[1].position = (v2) { position.x + size.x, position.y          };
  quad[2].position = (v2) { position.x + size.x, position.y + size.y };
  quad[3].position = (v2) { position.x,          position.y + size.y };

  quad[0].blend = blend;
  quad[1].blend = blend;
  quad[2].blend = blend;
  quad[3].blend = blend;

  renderer.requests[layer][BATCH_SHADER_QUAD] = array_list_grow(renderer.requests[layer][BATCH_SHADER_QUAD], 1);
  for (u32 i = 0; i < 4; i++) {
    renderer.requests[layer][BATCH_SHADER_QUAD][array_list_size(renderer.requests[layer][BATCH_SHADER_QUAD]) - 1][i] = quad[i];
  }

  renderer.quads_amount++;
}

/*
 *
 * *** Window and Context things ***
 *
 * */

static GLFWwindow *window;

extern void __conf(blib_config *config);
extern void __init(void);
extern void __loop(f32 dt);
extern void __draw(str *batch_shaders);
extern void __quit(void);

static void
window_create(void) {
  blib_config config;
  config.title                    = "Blib App";
  config.width                    = 640;
  config.height                   = 480;
  config.center                   = true;
  config.resizable                = false;
  config.quads_capacity           = 10000; /* 10000 quads */
  config.layers_amount            = 5;
  __conf(&config);
  renderer.vertices_capa = config.quads_capacity * 4;
  renderer.indices_capa  = config.quads_capacity * 6;
  renderer.layers_amount = config.layers_amount;

  if (!glfwInit()) {
    ccstr desc;
    glfwGetError(&desc);
    err("GLFW: %s\n", desc);
    exit(1);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_RESIZABLE, config.resizable ? GLFW_TRUE : GLFW_FALSE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if macintosh || Macintosh || (__APPLE__ && __MACH__)
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif
  if (config.width  <= 0) config.width  = 640;
  if (config.height <= 0) config.height = 480;
  window = glfwCreateWindow(config.width, config.height, config.title, 0, 0);
  if (!window) {
    ccstr desc;
    glfwGetError(&desc);
    err("GLFW: %s\n", desc);
    glfwTerminate();
    exit(1);
  }
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  if (config.center) {
    const GLFWvidmode *vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(window, 
        config.width  >= vidmode->width  ? 0 : (vidmode->width  >> 1) - (config.width  >> 1),
        config.height >= vidmode->height ? 0 : (vidmode->height >> 1) - (config.height >> 1));
  }
}

static void
window_destroy(void) {
  glfwTerminate();
}

s32
main(void) {
  window_create();

  entity_system_init();
  asset_manager_init();
  renderer_init();

  __init();
  while (!glfwWindowShouldClose(window)) {
    __loop(1.0f / 60.0f); /* TODO: proper delta time */
    __draw(renderer.batch_shaders);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  __quit();

  window_destroy();

  return 0;
}

