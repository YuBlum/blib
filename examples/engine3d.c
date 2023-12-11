#include <blib.h>
#include <stdio.h>

#define GAME_W 256
#define GAME_H 240
#define GAME_S 3

typedef struct {
  v3f p[3];
} triangle;
#define TRIANGLE(X1, Y1, Z1, \
                 X2, Y2, Z2, \
                 X3, Y3, Z3) ((triangle) { { { X1, Y1, Z1 }, { X2, Y2, Z2 }, { X3, Y3, Z3 } } })

typedef struct {
  triangle *tris;
} mesh;

static void
create_mesh(mesh *mesh) {
  mesh->tris = array_list_create(sizeof (triangle));
}

static mesh mesh_cube;
static m4 projection;
static v3f camera;

static v3f
m4_mul_v3f(m4 a, v3f b) {
  v4f c = m4_mul_v4f(a, V4F(b.x, b.y, b.z, 1));

  b = V3F(c.x, c.y, c.z);

  if (c.w != 0.0f) {
    return v3f_div_scalar(b, c.w);
  }

  return b;
}

static void
draw_triangle_outline(triangle t, v4f blend, u32 layer) {
  v2f p1 = V2F(t.p[0].x, t.p[0].y);
  v2f p2 = V2F(t.p[1].x, t.p[1].y);
  v2f p3 = V2F(t.p[2].x, t.p[2].y);
  draw_line(p1, p2, 1, blend, layer);
  draw_line(p2, p3, 1, blend, layer);
  draw_line(p3, p1, 1, blend, layer);
}

void
__conf(blib_config *config) {
  config->window_title = "3D Demo";
  config->game_width   = GAME_W;
  config->game_height  = GAME_H;
  config->game_scale   = GAME_S;
}

void
__init(void) {
  create_mesh(&mesh_cube);
  mesh_cube.tris = array_list_grow(mesh_cube.tris, 12);

  /* SOUTH */
  mesh_cube.tris[0] = TRIANGLE(0, 0, 0,   0, 1, 0,   1, 1, 0);
  mesh_cube.tris[1] = TRIANGLE(0, 0, 0,   1, 1, 0,   1, 0, 0);

  /* EAST */
  mesh_cube.tris[2] = TRIANGLE(1, 0, 0,   1, 1, 0,   1, 1, 1);
  mesh_cube.tris[3] = TRIANGLE(1, 0, 0,   1, 1, 1,   1, 0, 1);

  /* NORTH */
  mesh_cube.tris[4] = TRIANGLE(1, 0, 1,   1, 1, 1,   0, 1, 1);
  mesh_cube.tris[5] = TRIANGLE(1, 0, 1,   0, 1, 1,   0, 0, 1);

  /* WEST */
  mesh_cube.tris[6] = TRIANGLE(0, 0, 1,   0, 1, 1,   0, 1, 0);
  mesh_cube.tris[7] = TRIANGLE(0, 0, 1,   0, 1, 0,   0, 0, 0);

  /* TOP */
  mesh_cube.tris[8] = TRIANGLE(0, 1, 0,   0, 1, 1,   1, 1, 1);
  mesh_cube.tris[9] = TRIANGLE(0, 1, 0,   1, 1, 1,   1, 1, 0);

  /* BOTTOM */
  mesh_cube.tris[10] = TRIANGLE(0, 0, 1,   0, 0, 0,   1, 0, 0);
  mesh_cube.tris[11] = TRIANGLE(0, 0, 1,   1, 0, 0,   1, 0, 1);


  /* Projection Matrix */
  f32 near = 0.1f;
  f32 far  = 1000.0f;
  f32 fov  = 90.0f;
  f32 ar   = (f32)GAME_W / (f32)GAME_H;
  f32 fovr = atanf(DEG2RAD(fov * 0.5f));
  f32 z_eq = far/(far-near);

  projection = M4(
      ar*fovr,    0,    0,         0,
            0, fovr,    0,         0,
            0,    0, z_eq, -far*z_eq,
            0,    0,    1,          0
  );
}

static f32 theta;

void
__loop(f32 dt) {
  theta += dt;
}

void
__tick(void) {
}

void
__draw(void) {
  clear_screen(COL_BLACK);

  m3 rot_x = M3_0;
  rot_x._00 = cosf(theta);
  rot_x._10 = sinf(theta);
  rot_x._01 = -sinf(theta);
  rot_x._11 = cosf(theta);
  rot_x._22 = 1;

  m3 rot_z = M3_0;
  rot_z._00 = 1;
  rot_z._11 = cosf(theta * 0.5f);
  rot_z._12 = sinf(theta * 0.5f);
  rot_z._21 = -sinf(theta * 0.5f);
  rot_z._22 = cosf(theta * 0.5f);

  for (u32 i = 0; i < array_list_size(mesh_cube.tris); i++) {
    triangle final, transformed;

    transformed = mesh_cube.tris[i];

    /* rotate x */
    transformed.p[0] = m3_mul_v3f(rot_x, transformed.p[0]);
    transformed.p[1] = m3_mul_v3f(rot_x, transformed.p[1]);
    transformed.p[2] = m3_mul_v3f(rot_x, transformed.p[2]);

    /* rotate y */
    transformed.p[0] = m3_mul_v3f(rot_z, transformed.p[0]);
    transformed.p[1] = m3_mul_v3f(rot_z, transformed.p[1]);
    transformed.p[2] = m3_mul_v3f(rot_z, transformed.p[2]);

    /* translate */
    transformed.p[0].z += 3.0f;
    transformed.p[1].z += 3.0f;
    transformed.p[2].z += 3.0f;

    /* get triangle normal */
    v3f normal, line1, line2;
    line1 = v3f_sub(transformed.p[1], transformed.p[0]);
    line2 = v3f_sub(transformed.p[2], transformed.p[0]);
    normal = v3f_unit(v3f_cross(line1, line2));

    if (v3f_dot(normal, v3f_sub(transformed.p[0], camera)) < 0) {
      /* project */
      final.p[0] = m4_mul_v3f(projection, transformed.p[0]);
      final.p[1] = m4_mul_v3f(projection, transformed.p[1]);
      final.p[2] = m4_mul_v3f(projection, transformed.p[2]);

      final.p[0] = v3f_mul(final.p[0], V3F(GAME_W * 0.5f, GAME_H * 0.5f, 1));
      final.p[1] = v3f_mul(final.p[1], V3F(GAME_W * 0.5f, GAME_H * 0.5f, 1));
      final.p[2] = v3f_mul(final.p[2], V3F(GAME_W * 0.5f, GAME_H * 0.5f, 1));

      draw_triangle_outline(final, COL_WHITE, 0);
    }
  }

  submit_batch();
}

void
__quit(void) {
}
