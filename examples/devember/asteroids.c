#include <blib.h>
#include <assert.h>

#define GAME_W 960
#define GAME_H 720

#define GAME_LEFT   (-GAME_W * 0.5f)
#define GAME_RIGHT  (+GAME_W * 0.5f)
#define GAME_BOTTOM (-GAME_H * 0.5f)
#define GAME_TOP    (+GAME_H * 0.5f)

typedef struct {
  v2f pos;
  f32 angle;
} projectile;

typedef struct {
  v2f points[8];
  v2f pos;
  u32 cell_size;

  f32 angle;
  f32 speed;

  f32 left;
  f32 right;
  f32 bottom;
  f32 top;
} asteroid;

static v2f player_pos;
static v2f player_max_vel;
static v2f player_cur_vel;
static f32 player_vel_timer;
static f32 player_angle;
static s32 player_mov_dir;
static f32 player_flash_fire_timer;
static b8  player_flash_fire;
static u32 player_health;

static u32 player_score;

static projectile *player_projectiles;

#define PLAYER_PROJECTILE_SPEED 500

#define PLAYER_SIZE  V2F(32, 16)
#define PLAYER_SPEED 200
#define PLAYER_ROT_SPEED 5

#define PLAYER_FLASH_FIRE_SPEED 20

#define PLAYER_DAMPING_SPEED 2

#define ASTEROID_LARGE_CELL  32
#define ASTEROID_MEDIUM_CELL 16
#define ASTEROID_SMALL_CELL  8

static asteroid *asteroids;

#define TRANSFORM_POINT(P)\
  P = V2F(cosf(player_angle)*P.x - sinf(player_angle)*P.y,  \
          sinf(player_angle)*P.x + cosf(player_angle)*P.y); \
  P = v2f_add(P, player_pos)
/* end TRANSFORM_POINT */

static void
create_asteroid(v2f pos, f32 cell_size, f32 min_speed, f32 max_speed) {
  asteroid asteroid;
  asteroid.pos = pos;
  asteroid.speed = RAND_1() * (max_speed + min_speed) - min_speed;
  asteroid.angle = RAND_1() * 2 * PI;
  u32 points_amount = 0;
  asteroid.right  = +INFINITY;
  asteroid.left   = -INFINITY;
  asteroid.top    = +INFINITY;
  asteroid.bottom = -INFINITY;
  asteroid.cell_size = cell_size;
  for (s32 i = -1; i < 2; i++) {
    for (s32 j = -1; j < 2; j++) {
      if (i == 0 && j == 0) continue;
      asteroid.points[points_amount] = V2F(
        RAND_1() * cell_size + cell_size * i,
        RAND_1() * cell_size + cell_size * j
      );
      if (j == -1 && asteroid.points[points_amount].y > asteroid.bottom) {
        asteroid.bottom = asteroid.points[points_amount].y;
      }
      if (j == +1 && asteroid.points[points_amount].y < asteroid.top) {
        asteroid.top = asteroid.points[points_amount].y;
      }
      if (i == +1 && asteroid.points[points_amount].x < asteroid.right) {
        asteroid.right = asteroid.points[points_amount].x;
      }
      if (i == -1 && asteroid.points[points_amount].x > asteroid.left) {
        asteroid.left = asteroid.points[points_amount].x;
      }
      points_amount++;
    }
  }
  assert(points_amount == 8);
  array_list_push(asteroids, asteroid);
}

static void
player_reset(void) {
  player_pos       = V2F_0;
  player_angle     = DEG2RAD(90.0f);
  player_max_vel   = V2F_0;
  player_cur_vel   = V2F_0;
  player_vel_timer = 0;
  player_mov_dir   = 0;
}

static void
game_start(void) {
  player_reset();
  player_health = 3;
  player_score = 0;

  array_list_clear(player_projectiles);
  array_list_clear(asteroids);

  u32 asteroid_amount = rand() % 3 + 3; // 3 to 6
  for (u32 i = 0; i < asteroid_amount; i++) {
    v2f pos = {
      RAND_1() * GAME_W - GAME_W * 0.5f,
      RAND_1() * GAME_H - GAME_H * 0.5f
    };
    create_asteroid(pos, ASTEROID_LARGE_CELL, 100, 150);
  }
}

static void
player_move(f32 dt) {
  player_angle += dt * (key_press('A') - key_press('D')) * PLAYER_ROT_SPEED;
  player_mov_dir = key_press('W') - key_press('S');

  if (player_mov_dir != 0) {
    player_max_vel = V2F(
      player_mov_dir * cosf(player_angle) * PLAYER_SPEED,
      player_mov_dir * sinf(player_angle) * PLAYER_SPEED
    );
    player_vel_timer = 0;
    player_cur_vel = player_max_vel;

    player_flash_fire_timer += dt * PLAYER_FLASH_FIRE_SPEED;
    if (player_flash_fire_timer > 1.0f) {
      player_flash_fire_timer = 0.0f;
      player_flash_fire = !player_flash_fire;
    }
  } else if (player_cur_vel.x != 0 || player_cur_vel.y != 0) {
    player_vel_timer += dt * PLAYER_DAMPING_SPEED;
    player_cur_vel = v2f_lerp(player_max_vel, V2F_0, player_vel_timer);
    if (player_vel_timer >= 1.0f) {
      player_cur_vel = V2F_0;
      player_vel_timer = 0.0f;
    }
  }

  player_pos.x += dt * player_cur_vel.x;
  player_pos.y += dt * player_cur_vel.y;

  if (player_pos.x < GAME_LEFT)   player_pos.x = GAME_RIGHT;
  if (player_pos.x > GAME_RIGHT)  player_pos.x = GAME_LEFT;
  if (player_pos.y < GAME_BOTTOM) player_pos.y = GAME_TOP;
  if (player_pos.y > GAME_TOP)    player_pos.y = GAME_BOTTOM;
}

static void
player_shoot(f32 dt) {
  /* create projectile */
  if (key_click(' ')) {
    projectile projectile = {
      V2F(+PLAYER_SIZE.x * 0.50f, 0),
      player_angle
    };
    TRANSFORM_POINT(projectile.pos);
    array_list_push(player_projectiles, projectile);
  }

  /* update/destroy projectiles */
  for (u32 i = array_list_size(player_projectiles) - 1; i < (u32)-1; i--) {
    player_projectiles[i].pos.x +=
      dt * cosf(player_projectiles[i].angle) * PLAYER_PROJECTILE_SPEED;
    player_projectiles[i].pos.y +=
      dt * sinf(player_projectiles[i].angle) * PLAYER_PROJECTILE_SPEED;
    b8 hitted_asteroid = false;
    for (u32 j = array_list_size(asteroids) - 1; j < (u32)-1; j--) {
      if (player_projectiles[i].pos.x + 5 > asteroids[j].pos.x + asteroids[j].left  &&
          player_projectiles[i].pos.x - 5 < asteroids[j].pos.x + asteroids[j].right &&
          player_projectiles[i].pos.y - 5 < asteroids[j].pos.y + asteroids[j].top   &&
          player_projectiles[i].pos.y + 5 > asteroids[j].pos.y + asteroids[j].bottom) {
        switch (asteroids[j].cell_size) {
          case ASTEROID_LARGE_CELL:
            create_asteroid(asteroids[j].pos, ASTEROID_MEDIUM_CELL, 150, 200);
            create_asteroid(asteroids[j].pos, ASTEROID_MEDIUM_CELL, 150, 200);
            player_score += 10;
            break;
          case ASTEROID_MEDIUM_CELL:
            create_asteroid(asteroids[j].pos, ASTEROID_SMALL_CELL, 200, 250);
            create_asteroid(asteroids[j].pos, ASTEROID_SMALL_CELL, 200, 250);
            create_asteroid(asteroids[j].pos, ASTEROID_SMALL_CELL, 200, 250);
            create_asteroid(asteroids[j].pos, ASTEROID_SMALL_CELL, 200, 250);
            player_score += 20;
            break;
          case ASTEROID_SMALL_CELL:
            player_score += 40;
            break;
        }
        array_list_remove(asteroids, j, 0);
        array_list_remove(player_projectiles, i, 0);
      }
    }
    if (hitted_asteroid) continue;
    if (player_projectiles[i].pos.x > GAME_RIGHT  ||
        player_projectiles[i].pos.x < GAME_LEFT   ||
        player_projectiles[i].pos.y < GAME_BOTTOM ||
        player_projectiles[i].pos.y > GAME_TOP) {
      array_list_remove(player_projectiles, i, 0);
    }
  }
}

void
asteroids_move(f32 dt) {
  for (u32 i = 0; i < array_list_size(asteroids); i++) {
     asteroids[i].pos.x += dt * cosf(asteroids[i].angle) * asteroids[i].speed;
     asteroids[i].pos.y += dt * sinf(asteroids[i].angle) * asteroids[i].speed;
      if (asteroids[i].pos.x < GAME_LEFT)   asteroids[i].pos.x = GAME_RIGHT;
      if (asteroids[i].pos.x > GAME_RIGHT)  asteroids[i].pos.x = GAME_LEFT;
      if (asteroids[i].pos.y < GAME_BOTTOM) asteroids[i].pos.y = GAME_TOP;
      if (asteroids[i].pos.y > GAME_TOP)    asteroids[i].pos.y = GAME_BOTTOM;
  }
}

void
player_dies(void) {
  for (u32 j = array_list_size(asteroids) - 1; j < (u32)-1; j--) {
    if (player_pos.x + 9 > asteroids[j].pos.x + asteroids[j].left  &&
        player_pos.x - 9 < asteroids[j].pos.x + asteroids[j].right &&
        player_pos.y - 9 < asteroids[j].pos.y + asteroids[j].top   &&
        player_pos.y + 9 > asteroids[j].pos.y + asteroids[j].bottom) {
      switch (asteroids[j].cell_size) {
        case ASTEROID_LARGE_CELL:
          create_asteroid(asteroids[j].pos, ASTEROID_MEDIUM_CELL, 150, 200);
          create_asteroid(asteroids[j].pos, ASTEROID_MEDIUM_CELL, 150, 200);
          player_score += 5;
          break;
        case ASTEROID_MEDIUM_CELL:
          create_asteroid(asteroids[j].pos, ASTEROID_SMALL_CELL, 200, 250);
          create_asteroid(asteroids[j].pos, ASTEROID_SMALL_CELL, 200, 250);
          create_asteroid(asteroids[j].pos, ASTEROID_SMALL_CELL, 200, 250);
          create_asteroid(asteroids[j].pos, ASTEROID_SMALL_CELL, 200, 250);
          player_score += 10;
          break;
        case ASTEROID_SMALL_CELL:
          player_score += 20;
          break;
      }
      array_list_remove(asteroids, j, 0);
      player_reset();
      player_health--;
      if (player_health == 0) {
        game_start();
      }
    }
  }
}

void
__conf(blib_config *config) {
  config->window_title = "ASTEROIDS";
  config->game_width   = GAME_W;
  config->game_height  = GAME_H;
}

void
__init(void) {
  player_projectiles = array_list_create(sizeof (projectile));
  asteroids = array_list_create(sizeof (asteroid));
  game_start();
}

void
__loop(f32 dt) {  
  player_move(dt);
  player_shoot(dt);
  asteroids_move(dt);
  player_dies();
}

void
__tick(void) {
}

void
__draw(batch *batch) {
  clear_screen(COL_BLACK);

  /* draw player */
  v2f p1 = V2F(-PLAYER_SIZE.x * 0.50f, -PLAYER_SIZE.y * 0.50f);
  v2f p2 = V2F(-PLAYER_SIZE.x * 0.25f, 0);
  v2f p3 = V2F(-PLAYER_SIZE.x * 0.50f, +PLAYER_SIZE.y * 0.50f);
  v2f p4 = V2F(+PLAYER_SIZE.x * 0.50f, 0);

  TRANSFORM_POINT(p1);
  TRANSFORM_POINT(p2);
  TRANSFORM_POINT(p3);
  TRANSFORM_POINT(p4);

  draw_line(p1, p2, 2, COL_WHITE, 0);
  draw_line(p2, p3, 2, COL_WHITE, 0);
  draw_line(p3, p4, 2, COL_WHITE, 0);
  draw_line(p4, p1, 2, COL_WHITE, 0);

  if (player_mov_dir != 0 && player_flash_fire) {
    v2f f1 = v2f_lerp(p2, p1, 0.5f);
    v2f f2 = v2f_lerp(p2, p3, 0.5f);
    v2f f3 = V2F(-PLAYER_SIZE.x * 0.6f, 0);
    TRANSFORM_POINT(f3);
    draw_line(f2, f3, 2, COL_WHITE, 0);
    draw_line(f3, f1, 2, COL_WHITE, 0);
  }

  /* draw player projectile */
  for (u32 i = 0; i < array_list_size(player_projectiles); i++) {
    draw_rect(player_projectiles[i].pos, V2F(6, 2), V2F_0,
        player_projectiles[i].angle, COL_WHITE, 0);
  }

  /* draw asteroids */
  for (u32 i = 0; i < array_list_size(asteroids); i++) {
#define DRAW_ASTEROID_POINT(N1, N2) do { \
      v2f p1 = v2f_add(asteroids[i].points[N1], asteroids[i].pos);\
      v2f p2 = v2f_add(asteroids[i].points[N2], asteroids[i].pos);\
      draw_line(p1, p2, 2, COL_WHITE, 0);\
    } while (0)
    DRAW_ASTEROID_POINT(0, 1);
    DRAW_ASTEROID_POINT(1, 2);
    DRAW_ASTEROID_POINT(2, 4);
    DRAW_ASTEROID_POINT(4, 7);
    DRAW_ASTEROID_POINT(7, 6);
    DRAW_ASTEROID_POINT(6, 5);
    DRAW_ASTEROID_POINT(5, 3);
    DRAW_ASTEROID_POINT(3, 0);

#undef DRAW_ASTEROID_POINT
  }

  /* HUD: */
  draw_text(
      V2F(GAME_LEFT + 20, GAME_TOP - 20), V2F_S(3),
      COL_WHITE, 0, STR("LIVES: %u   SCORE: %u"), player_health,
      player_score);

  submit_batch();
}

void
__quit(void) {
}
