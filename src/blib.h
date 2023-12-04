#ifndef __BLIB_H__
#define __BLIB_H__

#include <math.h>
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

#define PI (3.14159265359)

#define DEG2RAD(ANG) (ANG * (180.0f / PI))
#define RAD2DEG(ANG) (ANG * (PI / 180.0f))

/*
 * *** Vectors ***
 * */

typedef struct { f32 x, y;       } v2f;
typedef struct { f32 x, y, z;    } v3f;
typedef struct { f32 x, y, z, w; } v4f;

typedef struct { s32 x, y;       } v2i;
typedef struct { s32 x, y, z;    } v3i;
typedef struct { s32 x, y, z, w; } v4i;

typedef struct { u32 x, y;       } v2u;
typedef struct { u32 x, y, z;    } v3u;
typedef struct { u32 x, y, z, w; } v4u;

#define V2F(X, Y)       ((v2f){ X, Y       })
#define V3F(X, Y, Z)    ((v3f){ X, Y, Z    })
#define V4F(X, Y, Z, W) ((v4f){ X, Y, Z, W })

#define V2I(X, Y)       ((v2i){ X, Y       })
#define V3I(X, Y, Z)    ((v3i){ X, Y, Z    })
#define V4I(X, Y, Z, W) ((v4i){ X, Y, Z, W })

#define V2U(X, Y)       ((v2u){ X, Y       })
#define V3U(X, Y, Z)    ((v3u){ X, Y, Z    })
#define V4U(X, Y, Z, W) ((v4u){ X, Y, Z, W })

#define V2F_0 V2F(0, 0)
#define V3F_0 V3F(0, 0, 0)
#define V4F_0 V4F(0, 0, 0, 0)

#define V2I_0 V2I(0, 0)
#define V3I_0 V3I(0, 0, 0)
#define V4I_0 V4I(0, 0, 0, 0)

#define V2U_0 V2U(0, 0)
#define V3U_0 V3U(0, 0, 0)
#define V4U_0 V4U(0, 0, 0, 0)

static inline v2f v2f_add(v2f a, v2f b) { return V2F(a.x+b.x, a.y+b.y);                   }
static inline v2f v2f_sub(v2f a, v2f b) { return V2F(a.x-b.x, a.y-b.y);                   }
static inline v2f v2f_mul(v2f a, v2f b) { return V2F(a.x*b.x, a.y*b.y);                   }
static inline v2f v2f_div(v2f a, v2f b) { return V2F(a.x/b.x, a.y/b.y);                   }
static inline v2f v2f_add_scalar(v2f a, f32 b) { return V2F(a.x+b, a.y+b);                }
static inline v2f v2f_sub_scalar(v2f a, f32 b) { return V2F(a.x-b, a.y-b);                }
static inline v2f v2f_mul_scalar(v2f a, f32 b) { return V2F(a.x*b, a.y*b);                }
static inline v2f v2f_div_scalar(v2f a, f32 b) { return V2F(a.x/b, a.y/b);                }
static inline f32 v2f_dot(v2f a, v2f b) { return a.x*b.x + a.y*b.y;                       }
static inline f32 v2f_mag(v2f a)        { return sqrtf(a.x*a.x + a.y*a.y);                }
static inline f32 v2f_dist(v2f a, v2f b){ return v2f_mag(v2f_sub(b, a));                  }
static inline v3f v3f_add(v3f a, v3f b) { return V3F(a.x+b.x, a.y+b.y, a.z+b.z);          }
static inline v3f v3f_sub(v3f a, v3f b) { return V3F(a.x-b.x, a.y-b.y, a.z-b.z);          }
static inline v3f v3f_mul(v3f a, v3f b) { return V3F(a.x*b.x, a.y*b.y, a.z*b.z);          }
static inline v3f v3f_div(v3f a, v3f b) { return V3F(a.x/b.x, a.y/b.y, a.z/b.z);          }
static inline v3f v3f_add_scalar(v3f a, f32 b) { return V3F(a.x+b, a.y+b, a.z+b);         }
static inline v3f v3f_sub_scalar(v3f a, f32 b) { return V3F(a.x-b, a.y-b, a.z-b);         }
static inline v3f v3f_mul_scalar(v3f a, f32 b) { return V3F(a.x*b, a.y*b, a.z*b);         }
static inline v3f v3f_div_scalar(v3f a, f32 b) { return V3F(a.x/b, a.y/b, a.z/b);         }
static inline f32 v3f_dot(v3f a, v3f b) { return a.x*b.x + a.y*b.y + a.z*b.z;             }
static inline f32 v3f_mag(v3f a)        { return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);      }
static inline f32 v3f_dist(v3f a, v3f b){ return v3f_mag(v3f_sub(b, a));                  }
static inline v4f v4f_add(v4f a, v4f b) { return V4F(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w); }
static inline v4f v4f_sub(v4f a, v4f b) { return V4F(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w); }
static inline v4f v4f_mul(v4f a, v4f b) { return V4F(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w); }
static inline v4f v4f_div(v4f a, v4f b) { return V4F(a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w); }
static inline v4f v4f_add_scalar(v4f a, f32 b) { return V4F(a.x+b, a.y+b, a.z+b, a.w+b);  }
static inline v4f v4f_sub_scalar(v4f a, f32 b) { return V4F(a.x-b, a.y-b, a.z-b, a.w-b);  }
static inline v4f v4f_mul_scalar(v4f a, f32 b) { return V4F(a.x*b, a.y*b, a.z*b, a.w*b);  }
static inline v4f v4f_div_scalar(v4f a, f32 b) { return V4F(a.x/b, a.y/b, a.z/b, a.w/b);  }
static inline f32 v4f_dot(v4f a, v4f b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;   }
static inline f32 v4f_mag(v4f a)        { return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);      }
static inline f32 v4f_dist(v4f a, v4f b){ return v4f_mag(v4f_sub(b, a));                  }

static inline v2i v2i_add(v2i a, v2i b) { return V2I(a.x+b.x, a.y+b.y);                   }
static inline v2i v2i_sub(v2i a, v2i b) { return V2I(a.x-b.x, a.y-b.y);                   }
static inline v2i v2i_mul(v2i a, v2i b) { return V2I(a.x*b.x, a.y*b.y);                   }
static inline v2i v2i_div(v2i a, v2i b) { return V2I(a.x/b.x, a.y/b.y);                   }
static inline v2i v2i_add_scalar(v2i a, f32 b) { return V2I(a.x+b, a.y+b);                }
static inline v2i v2i_sub_scalar(v2i a, f32 b) { return V2I(a.x-b, a.y-b);                }
static inline v2i v2i_mul_scalar(v2i a, f32 b) { return V2I(a.x*b, a.y*b);                }
static inline v2i v2i_div_scalar(v2i a, f32 b) { return V2I(a.x/b, a.y/b);                }
static inline f32 v2i_dot(v2i a, v2i b) { return a.x*b.x + a.y*b.y;                       }
static inline f32 v2i_mag(v2i a)        { return sqrtf(a.x*a.x + a.y*a.y);                }
static inline f32 v2i_dist(v2i a, v2i b){ return v2i_mag(v2i_sub(b, a));                  }
static inline v3i v3i_add(v3i a, v3i b) { return V3I(a.x+b.x, a.y+b.y, a.z+b.z);          }
static inline v3i v3i_sub(v3i a, v3i b) { return V3I(a.x-b.x, a.y-b.y, a.z-b.z);          }
static inline v3i v3i_mul(v3i a, v3i b) { return V3I(a.x*b.x, a.y*b.y, a.z*b.z);          }
static inline v3i v3i_div(v3i a, v3i b) { return V3I(a.x/b.x, a.y/b.y, a.z/b.z);          }
static inline v3i v3i_add_scalar(v3i a, f32 b) { return V3I(a.x+b, a.y+b, a.z+b);         }
static inline v3i v3i_sub_scalar(v3i a, f32 b) { return V3I(a.x-b, a.y-b, a.z-b);         }
static inline v3i v3i_mul_scalar(v3i a, f32 b) { return V3I(a.x*b, a.y*b, a.z*b);         }
static inline v3i v3i_div_scalar(v3i a, f32 b) { return V3I(a.x/b, a.y/b, a.z/b);         }
static inline f32 v3i_dot(v3i a, v3i b) { return a.x*b.x + a.y*b.y + a.z*b.z;             }
static inline f32 v3i_mag(v3i a)        { return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);      }
static inline f32 v3i_dist(v3i a, v3i b){ return v3i_mag(v3i_sub(b, a));                  }
static inline v4i v4i_add(v4i a, v4i b) { return V4I(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w); }
static inline v4i v4i_sub(v4i a, v4i b) { return V4I(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w); }
static inline v4i v4i_mul(v4i a, v4i b) { return V4I(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w); }
static inline v4i v4i_div(v4i a, v4i b) { return V4I(a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w); }
static inline v4i v4i_add_scalar(v4i a, f32 b) { return V4I(a.x+b, a.y+b, a.z+b, a.w+b);  }
static inline v4i v4i_sub_scalar(v4i a, f32 b) { return V4I(a.x-b, a.y-b, a.z-b, a.w-b);  }
static inline v4i v4i_mul_scalar(v4i a, f32 b) { return V4I(a.x*b, a.y*b, a.z*b, a.w*b);  }
static inline v4i v4i_div_scalar(v4i a, f32 b) { return V4I(a.x/b, a.y/b, a.z/b, a.w/b);  }
static inline f32 v4i_dot(v4i a, v4i b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;   }
static inline f32 v4i_mag(v4i a)        { return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);      }
static inline f32 v4i_dist(v4i a, v4i b){ return v4i_mag(v4i_sub(b, a));                  }

static inline v2u v2u_add(v2u a, v2u b) { return V2U(a.x+b.x, a.y+b.y);                   }
static inline v2u v2u_sub(v2u a, v2u b) { return V2U(a.x-b.x, a.y-b.y);                   }
static inline v2u v2u_mul(v2u a, v2u b) { return V2U(a.x*b.x, a.y*b.y);                   }
static inline v2u v2u_div(v2u a, v2u b) { return V2U(a.x/b.x, a.y/b.y);                   }
static inline v2u v2u_add_scalar(v2u a, f32 b) { return V2U(a.x+b, a.y+b);                }
static inline v2u v2u_sub_scalar(v2u a, f32 b) { return V2U(a.x-b, a.y-b);                }
static inline v2u v2u_mul_scalar(v2u a, f32 b) { return V2U(a.x*b, a.y*b);                }
static inline v2u v2u_div_scalar(v2u a, f32 b) { return V2U(a.x/b, a.y/b);                }
static inline f32 v2u_dot(v2u a, v2u b) { return a.x*b.x + a.y*b.y;                       }
static inline f32 v2u_mag(v2u a)        { return sqrtf(a.x*a.x + a.y*a.y);                }
static inline f32 v2u_dist(v2u a, v2u b){ return v2u_mag(v2u_sub(b, a));                  }
static inline v3u v3u_add(v3u a, v3u b) { return V3U(a.x+b.x, a.y+b.y, a.z+b.z);          }
static inline v3u v3u_sub(v3u a, v3u b) { return V3U(a.x-b.x, a.y-b.y, a.z-b.z);          }
static inline v3u v3u_mul(v3u a, v3u b) { return V3U(a.x*b.x, a.y*b.y, a.z*b.z);          }
static inline v3u v3u_div(v3u a, v3u b) { return V3U(a.x/b.x, a.y/b.y, a.z/b.z);          }
static inline v3u v3u_add_scalar(v3u a, f32 b) { return V3U(a.x+b, a.y+b, a.z+b);         }
static inline v3u v3u_sub_scalar(v3u a, f32 b) { return V3U(a.x-b, a.y-b, a.z-b);         }
static inline v3u v3u_mul_scalar(v3u a, f32 b) { return V3U(a.x*b, a.y*b, a.z*b);         }
static inline v3u v3u_div_scalar(v3u a, f32 b) { return V3U(a.x/b, a.y/b, a.z/b);         }
static inline f32 v3u_dot(v3u a, v3u b) { return a.x*b.x + a.y*b.y + a.z*b.z;             }
static inline f32 v3u_mag(v3u a)        { return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);      }
static inline f32 v3u_dist(v3u a, v3u b){ return v3u_mag(v3u_sub(b, a));                  }
static inline v4u v4u_add(v4u a, v4u b) { return V4U(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w); }
static inline v4u v4u_sub(v4u a, v4u b) { return V4U(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w); }
static inline v4u v4u_mul(v4u a, v4u b) { return V4U(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w); }
static inline v4u v4u_div(v4u a, v4u b) { return V4U(a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w); }
static inline v4u v4u_add_scalar(v4u a, f32 b) { return V4U(a.x+b, a.y+b, a.z+b, a.w+b);  }
static inline v4u v4u_sub_scalar(v4u a, f32 b) { return V4U(a.x-b, a.y-b, a.z-b, a.w-b);  }
static inline v4u v4u_mul_scalar(v4u a, f32 b) { return V4U(a.x*b, a.y*b, a.z*b, a.w*b);  }
static inline v4u v4u_div_scalar(v4u a, f32 b) { return V4U(a.x/b, a.y/b, a.z/b, a.w/b);  }
static inline f32 v4u_dot(v4u a, v4u b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;   }
static inline f32 v4u_mag(v4u a)        { return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);      }
static inline f32 v4u_dist(v4u a, v4u b){ return v4u_mag(v4u_sub(b, a));                  }

/*
 * *** Matrices ***
 * */

typedef struct {
  f32 _00, _01,
      _10, _11;
} m2;

typedef struct {
  f32 _00, _01, _02,
      _10, _11, _12,
      _20, _21, _22;
} m3;

typedef struct {
  f32 _00, _01, _02, _03,
      _10, _11, _12, _13,
      _20, _21, _22, _23,
      _30, _31, _32, _33;
} m4;

#define M2(_00, _01, \
           _10, _11) (m2) { _00, _10, \
                            _01, _11 }

#define M3(_00, _01, _02, \
           _10, _11, _12, \
           _20, _21, _22) (m3) { _00, _01, _02, \
                                 _10, _11, _12, \
                                 _20, _21, _22 }
#define M4(_00, _01, _02, _03, \
           _10, _11, _12, _13, \
           _20, _21, _22, _23, \
           _30, _31, _32, _33) (m4) { _00, _01, _02, _03, \
                                      _10, _11, _12, _13, \
                                      _20, _21, _22, _23, \
                                      _30, _31, _32, _33 }

#define M2_ID M2(1, 0, \
                 0, 1)

#define M3_ID M3(1, 0, 0, \
                 0, 1, 0, \
                 0, 0, 1)

#define M4_ID M4(1, 0, 0, 0, \
                 0, 1, 0, 0, \
                 0, 0, 1, 0, \
                 0, 0, 0, 1)

static inline m2
m2_mul(m2 a, m2 b) {
  m2 result;

  result._00 = a._00*b._00 + a._01*b._10;
  result._10 = a._00*b._01 + a._01*b._11;

  result._01 = a._10*b._00 + a._11*b._10;
  result._11 = a._10*b._01 + a._11*b._11;

  return result;
}

static inline m3
m3_mul(m3 a, m3 b) {
  m3 result;

  result._00 = a._00*b._00 + a._01*b._10 + a._02*b._20;
  result._10 = a._00*b._01 + a._01*b._11 + a._02*b._21;
  result._20 = a._00*b._02 + a._01*b._12 + a._02*b._22;

  result._01 = a._10*b._00 + a._11*b._10 + a._12*b._20;
  result._11 = a._10*b._01 + a._11*b._11 + a._12*b._21;
  result._21 = a._10*b._02 + a._11*b._12 + a._12*b._22;

  result._02 = a._20*b._00 + a._21*b._10 + a._22*b._20;
  result._12 = a._20*b._01 + a._21*b._11 + a._22*b._21;
  result._22 = a._20*b._02 + a._21*b._12 + a._22*b._22;

  return result;
}

static inline m4
m4_mul(m4 a, m4 b) {
  m4 result;

  result._00 = a._00*b._00 + a._01*b._10 + a._02*b._20 + a._03*b._30;
  result._10 = a._00*b._01 + a._01*b._11 + a._02*b._21 + a._03*b._31;
  result._20 = a._00*b._02 + a._01*b._11 + a._02*b._22 + a._03*b._32;
  result._30 = a._00*b._03 + a._01*b._11 + a._02*b._22 + a._03*b._33;

  result._01 = a._10*b._00 + a._11*b._10 + a._12*b._20 + a._13*b._30;
  result._11 = a._10*b._01 + a._11*b._11 + a._12*b._21 + a._13*b._31;
  result._21 = a._10*b._02 + a._11*b._12 + a._12*b._22 + a._13*b._32;
  result._31 = a._10*b._03 + a._11*b._13 + a._12*b._23 + a._13*b._33;

  result._02 = a._20*b._00 + a._21*b._10 + a._22*b._20 + a._23*b._30;
  result._12 = a._20*b._01 + a._21*b._11 + a._22*b._21 + a._23*b._31;
  result._22 = a._20*b._02 + a._21*b._12 + a._22*b._22 + a._23*b._32;
  result._32 = a._20*b._03 + a._21*b._13 + a._22*b._23 + a._23*b._33;

  result._03 = a._30*b._00 + a._31*b._10 + a._32*b._20 + a._33*b._30;
  result._13 = a._30*b._01 + a._31*b._11 + a._32*b._21 + a._33*b._31;
  result._23 = a._30*b._02 + a._31*b._12 + a._32*b._22 + a._33*b._32;
  result._33 = a._30*b._03 + a._31*b._13 + a._32*b._23 + a._33*b._33;

  return result;
}

static inline v2f
m2_mul_v2f(m2 a, v2f b) {
  v2f result;

  result.x = a._00 * b.x + a._01 * b.y;
  result.y = a._10 * b.x + a._11 * b.y;

  return result;
}

static inline v3f
m3_mul_v3f(m3 a, v3f b) {
  v3f result;

  result.x = a._00*b.x + a._01*b.y + a._02*b.z;
  result.y = a._10*b.x + a._11*b.y + a._12*b.z;
  result.z = a._20*b.x + a._21*b.y + a._22*b.z;

  return result;
}

static inline v4f
m4_mul_v4f(m4 a, v4f b) {
  v4f result;

  result.x = a._00*b.x + a._01*b.y + a._02*b.z + a._03*b.w;
  result.y = a._10*b.x + a._11*b.y + a._12*b.z + a._13*b.w;
  result.z = a._20*b.x + a._21*b.y + a._22*b.z + a._23*b.w;
  result.w = a._30*b.x + a._31*b.y + a._32*b.z + a._33*b.w;

  return result;
}

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
#define CONST_STR(S) { sizeof (S) - 1, 0, S }
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
  ASSET_ATLAS,
  ASSET_SPRITE_FONT
} asset_type;

/* Loads an asset into memory.
 * Asset types:
 *   Shader:
 *     A directory with the shader's name must exists in 'assets/shaders/', inside the directory
 *     two files will be searched: vertex.glsl and fragment.glsl.
 *   Atlas:
 *     All atlases must be placed inside 'assets/atlases/'.
 *     supported atlases formats:
 *       png
 *       tga
 *       bmp
 *       jpg/jpeg
 *    Sprite Fonts:
 *     All sprite fonts must be placed inside 'assets/spritefonts/'.
 *     Sprite fonts only supports full ascii in a row for now
 *     (look 'assets/spritefonts/default.png' for an example)
 *     supported sprite fonts formats:
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

/* Set shader to `use` or not `use` the camera projection.
 * In case the camera projection is used a mat3 uniform with the name `u_camera`
 * must exist in the shader.*/
extern void shader_use_camera(str shader_name, b8 use);

/* Gets an uniform location of a shader */
extern uniform shader_get_uniform(str shader_name, str uniform_name);

/* Sets an integer `uniform` of the current setted shader to `value`. */
extern void shader_set_uniform_int(uniform uniform, s32 value);

/* Sets an unsigned integer `uniform` of the current setted shader  to `value`. */
extern void shader_set_uniform_uint(uniform uniform, u32 value);

/* Sets an float `uniform` of the current setted shader to `value`. */
extern void shader_set_uniform_float(uniform uniform, f32 value);

/* Sets an vector 2 `uniform` of the current setted shader to `value`. */
extern void shader_set_uniform_v2f(uniform uniform, v2f value);

/* Sets an vector 3 `uniform` of the current setted shader to `value`. */
extern void shader_set_uniform_v3f(uniform uniform, v3f value);

/* Sets an vector 4 `uniform` of the current setted shader to `value`. */
extern void shader_set_uniform_v4f(uniform uniform, v4f value);

/* Sets an vector 2 `uniform` of the current setted shader to `value`. */
extern void shader_set_uniform_v2i(uniform uniform, v2i value);

/* Sets an vector 3 `uniform` of the current setted shader to `value`. */
extern void shader_set_uniform_v3i(uniform uniform, v3i value);

/* Sets an vector 4 `uniform` of the current setted shader to `value`. */
extern void shader_set_uniform_v4i(uniform uniform, v4i value);

/* Sets an vector 2 `uniform` of the current setted shader to `value`. */
extern void shader_set_uniform_v2u(uniform uniform, v2u value);

/* Sets an vector 3 `uniform` of the current setted shader to `value`. */
extern void shader_set_uniform_v3u(uniform uniform, v3u value);

/* Sets an vector 4 `uniform` of the current setted shader to `value`. */
extern void shader_set_uniform_v4u(uniform uniform, v4u value);

/* Sets an matrix 2x2 `uniform` of the current setted shader to `value`. */
extern void shader_set_uniform_m2(uniform uniform, m2 value);

/* Sets an matrix 3x3 `uniform` of the current setted shader to `value`. */
extern void shader_set_uniform_m3(uniform uniform, m3 value);

/* Sets an matrix 4x4 `uniform` of the current setted shader to `value`. */
extern void shader_set_uniform_m4(uniform uniform, m4 value);

/* Sets an integer array `uniform` of the current setted shader to `values`. */
extern void shader_set_uniform_int_array(uniform uniform, s32 *values, u32 amount);

/* Sets an unsigned integer array `uniform` of the current setted shader to `values`. */
extern void shader_set_uniform_uint_array(uniform uniform, u32 *values, u32 amount);

/* Sets an float array `uniform` of the current setted shader to `values`. */
extern void shader_set_uniform_float_array(uniform uniform, f32 *values, u32 amount);

/* Sets an vector 2 array `uniform` of the current setted shader to `values`. */
extern void shader_set_uniform_v2f_array(uniform uniform, v2f *values, u32 amount);

/* Sets an vector 3 array `uniform` of the current setted shader to `values`. */
extern void shader_set_uniform_v3f_array(uniform uniform, v3f *values, u32 amount);

/* Sets an vector 4 array `uniform` of the current setted shader to `values`. */
extern void shader_set_uniform_v4f_array(uniform uniform, v4f *values, u32 amount);

/* Sets an vector 2 array `uniform` of the current setted shader to `values`. */
extern void shader_set_uniform_v2i_array(uniform uniform, v2i *values, u32 amount);

/* Sets an vector 3 array `uniform` of the current setted shader to `values`. */
extern void shader_set_uniform_v3i_array(uniform uniform, v3i *values, u32 amount);

/* Sets an vector 4 array `uniform` of the current setted shader to `values`. */
extern void shader_set_uniform_v4i_array(uniform uniform, v4i *values, u32 amount);

/* Sets an vector 2 array `uniform` of the current setted shader to `values`. */
extern void shader_set_uniform_v2u_array(uniform uniform, v2u *values, u32 amount);

/* Sets an vector 3 array `uniform` of the current setted shader to `values`. */
extern void shader_set_uniform_v3u_array(uniform uniform, v3u *values, u32 amount);

/* Sets an vector 4 array `uniform` of the current setted shader to `values`. */
extern void shader_set_uniform_v4u_array(uniform uniform, v4u *values, u32 amount);

/* Sets an matrix 2x2 array `uniform` of the current setted shader to `values`. */
extern void shader_set_uniform_m2_array(uniform uniform, m2 *values, u32 amount);

/* Sets an matrix 3x3 array `uniform` of the current setted shader to `values`. */
extern void shader_set_uniform_m3_array(uniform uniform, m3 *values, u32 amount);

/* Sets an matrix 4x4 array `uniform` of the current setted shader to `values`. */
extern void shader_set_uniform_m4_array(uniform uniform, m4 *values, u32 amount);

/*
 * *** Texture Atlas ***
 */

typedef u32 texture_id;

/* Setups the general info of `atlas`. */
extern void texture_atlas_setup(str atlas, u32 tile_width, u32 tile_height, u32 padding_x, u32 padding_y);

/* Gets the `atlas` texture id. */
extern texture_id texture_atlas_get_id(str atlas);

/*
 * *** Sprite Font ***
 */

/* Setups the general info of `font`. */
extern void sprite_font_setup(str font, u32 char_width, u32 char_height, u32 padding_x, u32 padding_y);

/* Gets the `font` texture id. */
extern texture_id sprite_font_get_id(str font);

/*
 * *** Texture Buffer
 */

typedef union {
  struct {
    u8 b, g, r, a; /* BGRA is better for the hex representation. */
  } color;
  u32 hex; /* AA RR GG BB */
} pixel;


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

/* Destroys a texture buffer. */
extern void texture_buff_destroy(pixel *buff);

/*
 * *** Camera ***
 */

/* Sets the camera position. */
extern void camera_set_position(v2f position);

/* Gets the camera position. */
extern v2f  camera_get_position(void);

/* Sets the camera scale. */
extern void camera_set_scale(v2f scale);

/* Gets the camera scale. */
extern v2f  camera_get_scale(void);

/* Sets the camera angle in radians. */
extern void camera_set_angle(f32 angle);

/* Gets the camera angle in radians. */
extern f32 camera_get_angle(void);

/*
 * *** Rendering ***
 */

typedef enum {
  BATCH_SHADER_QUAD = 0,
  BATCH_SHADER_ATLAS,
  BATCH_SHADER_FONT,
  BATCH_SHADER_TEXBUFF,
  BATCH_SHADERS_AMOUNT
} batch_shader_type;

typedef struct {
  str shaders[BATCH_SHADERS_AMOUNT];
  str atlas;
  str font;
  pixel *texture_buff;
} batch;

#define DEFAULT_SHADER_QUAD    STR("quad")
#define DEFAULT_SHADER_TEXTURE STR("texture")
#define DEFAULT_SHADER_ATLAS   DEFAULT_SHADER_TEXTURE
#define DEFAULT_SHADER_FONT    DEFAULT_SHADER_TEXTURE 
#define DEFAULT_SHADER_TEXBUFF DEFAULT_SHADER_TEXTURE 
#define DEFAULT_SPRITE_FONT    STR("default")

typedef v2f quad_texture_coords[4];

#define COL_WHITE        V4F(1.00f, 1.00f, 1.00f, 1.00f)
#define COL_BLACK        V4F(0.00f, 0.00f, 0.00f, 1.00f)
#define COL_GRAY         V4F(0.50f, 0.50f, 0.50f, 1.00f)
#define COL_LIGHT_GRAY   V4F(0.75f, 0.75f, 0.75f, 1.00f)
#define COL_DARK_GRAY    V4F(0.25f, 0.25f, 0.25f, 1.00f)
#define COL_RED          V4F(1.00f, 0.00f, 0.00f, 1.00f)
#define COL_GREEN        V4F(0.00f, 1.00f, 0.00f, 1.00f)
#define COL_BLUE         V4F(0.00f, 0.00f, 1.00f, 1.00f)
#define COL_YELLOW       V4F(1.00f, 1.00f, 0.00f, 1.00f)
#define COL_MAGENTA      V4F(1.00f, 0.00f, 1.00f, 1.00f)

/* Submits the current rendering batch into the screen. */
extern void submit_batch(void);

/* Clears the screen with `color` */
extern void clear_screen(v4f color);

/* Draws a quad into the screen */
extern void draw_quad(v2f position, v2f size, v4f blend, u32 layer);

/* Draws a tile of the current batch texture. */
extern void draw_tile(v2u tile, v2f position, v2f scale, v4f blend, u32 layer);

/* Draws a text into the screen, the text must have 512 characters only. */
extern void draw_text(v2f position, v2f scale, v4f blend, u32 layer, str fmt, ...);

/* Draws a part of the current batch texture buffer.
 * `parts` must be a array with 4 elements of v2f,
 * in case `parts` is NULL the full texture buffer is drawn.
 * */
extern void draw_texture_buff(v2f position, v2f size, v4f blend, u32 layer, v2f *parts);

/*
 * *** Input ***
 */

typedef enum {
  KEY_RIGHT,
  KEY_LEFT,
  KEY_UP,
  KEY_DOWN,
  KEY_LSHIFT,
  KEY_LCONTROL,
  KEY_LALT,
  KEY_RSHIFT,
  KEY_BACKSPACE, // '\b' can be used to denote the backspace key
  KEY_TAB, // '\t' can be used to denote the tab key
  KEY_RCONTROL,
  KEY_RALT,
  KEY_ESCAPE,
  KEY_RETURN, // '\r' can be used to denote the return key
  KEY_F1,
  KEY_F2,
  KEY_F3,
  KEY_F4,
  KEY_F5,
  KEY_F6,
  KEY_F7,
  KEY_F8,
  KEY_F9,
  KEY_F10,
  KEY_F11,
  KEY_F12,
  KEY_LAST,

  BTN_LEFT = 0,
  BTN_RIGHT,
  BTN_MIDDLE,
  BTN_CAP
} input_index;

/* Check if keyboard `key` was pressed. */
extern b8  key_press(input_index key);

/* Check if keyboard `key` was clicked. (Doesn't work in __tick) */
extern b8  key_click(input_index key);

/* Check if keyboard `key` was clicked. (works in __tick) */
extern b8  key_click_tick(input_index key);

/* Check if mouse `button` was pressed. */
extern b8  button_press(input_index button);

/* Check if mouse `button` was clicked. (Doesn't work in __tick) */
extern b8  button_click(input_index button);

/* Check if mouse `button` was clicked. (works in __tick) */
extern b8  button_click_tick(input_index button);

/* Gets mouse position relative to the game camera. */
extern v2f mouse_get_position(void);

/* Gets mouse position relative to the window. */
extern v2f mouse_get_screen_position(void);

/* Gets mouse scroll. */
extern v2f mouse_get_scroll(void);

/*
 * Window
 */

/* Closes the application window. */
extern void close_window(void);

/* Enables or disables vsync. */
extern void enable_vsync(b8 enable);

/*
 * A configuration struct to setup the app
 * */
typedef struct {
  cstr window_title;
  s32  window_width;
  s32  window_height;
  b8   window_center;
  b8   window_resizable;
  s32  camera_width;
  s32  camera_height;
  u32  quads_capacity;
  u32  layers_amount;
  u32  ticks_per_second;
} blib_config;

#endif/*__BLIB_H__*/
