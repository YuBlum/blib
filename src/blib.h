#ifndef __BLIB_H__
#define __BLIB_H__

#include <stdint.h>

/**** basic types ****/
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
typedef char     *cstr;

#define true  ((b8)1)
#define false ((b8)0)

b8 window_init(u32 width, u32 height, b8 resizable, cstr name);

#endif/*__BLIB_H__*/
