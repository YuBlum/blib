#include "blib.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define inf(...) fprintf(stderr, "Info:  " __VA_ARGS__)
#define wrn(...) fprintf(stderr, "Warn:  " __VA_ARGS__)
#define err(...) fprintf(stderr, "Error: " __VA_ARGS__)


/*
 *
 * *** Window and Context things ***
 *
 * */

static struct {
  b8 compatibility_profile;
  u32 major;
  u32 minor;
} opengl_version = {
  false,
  3, 3
};

static GLFWwindow *window;

void
set_opengl_version(u32 major, u32 minor, b8 compatibility_profile) {
  opengl_version.compatibility_profile = compatibility_profile;
  opengl_version.major = major;
  opengl_version.minor = minor;
}

b8
window_init(s32 width, s32 height, cstr title, b8 resizable, b8 center) {
  if (!glfwInit()) {
    ccstr desc;
    glfwGetError(&desc);
    err("GLFW: %s\n", desc);
    return false;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opengl_version.major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opengl_version.minor);
  glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
  glfwWindowHint(GLFW_OPENGL_PROFILE,
      opengl_version.compatibility_profile ? GLFW_OPENGL_COMPAT_PROFILE : GLFW_OPENGL_CORE_PROFILE);
#if macintosh || Macintosh || (__APPLE__ && __MACH__)
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif
  if (width  <= 0) width  = 640;
  if (height <= 0) height = 480;
  window = glfwCreateWindow(width, height, title, 0, 0);
  if (!window) {
    ccstr desc;
    glfwGetError(&desc);
    err("GLFW: %s\n", desc);
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(window);
  if (center) {
    const GLFWvidmode *vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(window, 
        width  >= vidmode->width  ? 0 : (vidmode->width  >> 1) - (width  >> 1),
        height >= vidmode->height ? 0 : (vidmode->height >> 1) - (height >> 1));
  }
  return true;
}

b8
window_is_running(void) {
  glfwSwapBuffers(window);
  glfwPollEvents();
  return !glfwWindowShouldClose(window);
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
#define ARRAY_LIST_HEADER(ARR) (((array_list_header *)arr) - 1)

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
array_list_grow(void *arr, u32 amount) {
  array_list_header *header = ARRAY_LIST_HEADER(arr);
  header->size += amount;
  if (header->size >= header->capa) {
    header->capa = header->size * 2;
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
array_list_destroy(void *arr) {
  free(ARRAY_LIST_HEADER(arr));
}
