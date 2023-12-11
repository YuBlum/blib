#include <blib.h>
#include <assert.h>

typedef struct {
  v2f size;
  f32 top,
      left,
      right,
      bottom;
} collider;

typedef struct {
  u32 type;
  v2f pos;
  b8 die_outside_left;
  f32 vel;
  collider col;
} car;


#define GAME_W 320
#define GAME_H 224
#define GAME_S 3

#define FROG_SPEED 4.0f
static v2f frog_prv_pos;
static v2f frog_cur_pos;
static v2f frog_nxt_pos;
static f32 frog_time;
static f32 frog_angle;
static b8  frog_dead;
static b8  frog_moving;
static collider frog_col;

static car *car_list;

#define CAR_SPAWN_TIMER_SPEED 1.5
static f32 car_spawn_timer;

static void
collider_update(collider *col, v2f pos) {
  col->top    = pos.y + col->size.y * 0.5f;
  col->bottom = pos.y - col->size.y * 0.5f;

  col->right = pos.x + col->size.x * 0.5f;
  col->left  = pos.x - col->size.x * 0.5f;
}

void
__conf(blib_config *config)  {
  config->window_title = "FROGGER";
  config->game_width   = GAME_W;
  config->game_height  = GAME_H;
  config->game_scale   = GAME_S;
}


void
game_start(void) {
  frog_cur_pos = V2F(8, -GAME_H * 0.5f + 8);
  frog_prv_pos = frog_cur_pos;
  frog_nxt_pos = frog_cur_pos;
  frog_moving  = false;
  frog_time    = 0;
  frog_angle   = 0;
  frog_dead    = false;
  frog_col.size = V2F(10, 10);
  collider_update(&frog_col, frog_cur_pos);
}

void
__init(void) {
  asset_load(ASSET_ATLAS, STR("frogger"));
  car_list = array_list_create(sizeof (car));

  game_start();
}

void
__loop(f32 dt) {  
  if (!frog_dead) {
    if (!frog_moving) {
      s32 dir_x = key_press('D') - key_press('A');
      s32 dir_y = key_press('W') - key_press('S');

      if (dir_y > 0) {
        frog_moving = true;
        frog_angle = DEG2RAD(0);
        frog_nxt_pos.y = frog_cur_pos.y + 16;
        if (frog_nxt_pos.y > GAME_H * 0.5f) {
          frog_nxt_pos.y = frog_cur_pos.y;
          frog_moving = false;
        }
      } else if (dir_y < 0) {
        frog_moving = true;
        frog_nxt_pos.y = frog_cur_pos.y - 16;
        frog_angle = DEG2RAD(180);
        if (frog_nxt_pos.y < -GAME_H * 0.5f) {
          frog_nxt_pos.y = frog_cur_pos.y;
          frog_moving = false;
        }
      } else if (dir_x > 0) {
        frog_moving = true;
        frog_nxt_pos.x = frog_cur_pos.x + 16;
        frog_angle = DEG2RAD(270);
        if (frog_nxt_pos.x > GAME_W * 0.5f) {
          frog_nxt_pos.x = frog_cur_pos.x;
          frog_moving = false;
        }
      } else if (dir_x < 0) {
        frog_moving = true;
        frog_nxt_pos.x = frog_cur_pos.x - 16;
        frog_angle = DEG2RAD(90);
        if (frog_nxt_pos.x < -GAME_W * 0.5f) {
          frog_nxt_pos.x = frog_cur_pos.x;
          frog_moving = false;
        }
      }
    } else {
      frog_time += dt * FROG_SPEED;
      if (frog_time >= 1.0f) {
        frog_time = 0.0f;
        frog_prv_pos = frog_cur_pos;
        frog_cur_pos = frog_nxt_pos;
        frog_moving = false;
      } else {
        frog_cur_pos = v2f_lerp(frog_prv_pos, frog_nxt_pos, frog_time);
      }
    }
    collider_update(&frog_col, frog_cur_pos);
  } else if (key_click(' ')) game_start();

  car_spawn_timer += dt * CAR_SPAWN_TIMER_SPEED;
  if (car_spawn_timer > 1.0f) {
    car_spawn_timer = 0.0f;
    u32 car_amount = rand() % 5 + 1;
    for (u32 i = 0; i < 5; i++) {
      if (rand() % 2 == 0) continue;
      car_amount--;
      car car = { 0 };
      car.type = i;
      switch (car.type) {
        case 0:
          car.pos = V2F(GAME_W * 0.5f + 16, -GAME_H * 0.5f + 16 + 32 * 1);
          car.vel = -100;
          car.die_outside_left = true;
          car.col.size = V2F(32, 32);
          break;
        case 1:
          car.pos = V2F(GAME_W * 0.5f + 16, -GAME_H * 0.5f + 16 + 32 * 2);
          car.vel = -80;
          car.die_outside_left = true;
          car.col.size = V2F(32, 32);
          break;
        case 2:
          car.pos = V2F(GAME_W * 0.5f + 16, -GAME_H * 0.5f + 16 + 32 * 3);
          car.vel = -110;
          car.die_outside_left = true;
          car.col.size = V2F(32, 32);
          break;
        case 3:
          car.pos = V2F(-GAME_W * 0.5f - 16, -GAME_H * 0.5f + 16 + 32 * 4);
          car.vel = 100;
          car.die_outside_left = false;
          car.col.size = V2F(32, 32);
          break;
        case 4:
          car.pos = V2F(GAME_W * 0.5f + 32, -GAME_H * 0.5f + 16 + 32 * 5);
          car.vel = -110;
          car.die_outside_left = true;
          car.col.size = V2F(64, 32);
          break;
      default:
        assert(false);
        break;
      }
      array_list_push(car_list, car);
      if  (car_amount == 0) break;
    }
  }
  
  for (u32 i = array_list_size(car_list) - 1; i < (u32)-1; i--) {
    car_list[i].pos.x += car_list[i].vel * dt;
    collider_update(&car_list[i].col, car_list[i].pos);
    if ((car_list[i].die_outside_left  && car_list[i].pos.x <= -GAME_W * 0.5f - 32.0f) ||
        (!car_list[i].die_outside_left && car_list[i].pos.x >=  GAME_W * 0.5f + 32.0f)) {
      array_list_remove(car_list, i, 0);
    } else if (
        frog_col.right  > car_list[i].col.left &&
        frog_col.left   < car_list[i].col.right &&
        frog_col.bottom < car_list[i].col.top &&
        frog_col.top > car_list[i].col.bottom) {
      frog_dead = true;
    }
  }
}

void
__tick(void) {
}

void
__draw(batch *batch) {
  batch->atlas = STR("frogger");
  clear_screen(COL_BLACK);
  for (u32 i = 0; i < GAME_W / 16; i++) {
    draw_tile(V2U(0, 1), V2F(-GAME_W * 0.5f + i * 16 + 8, -GAME_H * 0.5f + 16 + 8), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
    draw_tile(V2U(0, 1), V2F(-GAME_W * 0.5f + i * 16 + 8,  GAME_H * 0.5f - 16 - 8), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
  }
  if (frog_dead) {
    draw_tile(V2U(1, 1), frog_cur_pos, V2F(1, 1), V2F_0, frog_angle, COL_WHITE, 0);
  } else {
    draw_tile(V2U(frog_moving, 0), frog_cur_pos, V2F(1, 1), V2F_0, frog_angle, COL_WHITE, 0);
  }
  for (u32 i = array_list_size(car_list) - 1; i < (u32)-1; i--) {
    switch (car_list[i].type) {
      case 0:
        draw_tile(V2U(2, 0), v2f_add(car_list[i].pos, V2F(-8, +8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        draw_tile(V2U(3, 0), v2f_add(car_list[i].pos, V2F(+8, +8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        draw_tile(V2U(2, 1), v2f_add(car_list[i].pos, V2F(-8, -8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        draw_tile(V2U(3, 1), v2f_add(car_list[i].pos, V2F(+8, -8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        break;
      case 1:
        draw_tile(V2U(0, 2), v2f_add(car_list[i].pos, V2F(-8, +8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        draw_tile(V2U(1, 2), v2f_add(car_list[i].pos, V2F(+8, +8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        draw_tile(V2U(0, 3), v2f_add(car_list[i].pos, V2F(-8, -8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        draw_tile(V2U(1, 3), v2f_add(car_list[i].pos, V2F(+8, -8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        break;
      case 2:
        draw_tile(V2U(2, 2), v2f_add(car_list[i].pos, V2F(-8, +8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        draw_tile(V2U(3, 2), v2f_add(car_list[i].pos, V2F(+8, +8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        draw_tile(V2U(2, 3), v2f_add(car_list[i].pos, V2F(-8, -8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        draw_tile(V2U(3, 3), v2f_add(car_list[i].pos, V2F(+8, -8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        break;
      case 3:
        draw_tile(V2U(4, 0), v2f_add(car_list[i].pos, V2F(-8, +8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        draw_tile(V2U(5, 0), v2f_add(car_list[i].pos, V2F(+8, +8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        draw_tile(V2U(4, 1), v2f_add(car_list[i].pos, V2F(-8, -8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        draw_tile(V2U(5, 1), v2f_add(car_list[i].pos, V2F(+8, -8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        break;
      case 4:
        draw_tile(V2U(1, 4), v2f_add(car_list[i].pos, V2F(-8, +8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        draw_tile(V2U(2, 4), v2f_add(car_list[i].pos, V2F(+8, +8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        draw_tile(V2U(1, 5), v2f_add(car_list[i].pos, V2F(-8, -8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        draw_tile(V2U(2, 5), v2f_add(car_list[i].pos, V2F(+8, -8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);

        draw_tile(V2U(0, 4), v2f_add(car_list[i].pos, V2F(-24, +8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        draw_tile(V2U(3, 4), v2f_add(car_list[i].pos, V2F(+24, +8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        draw_tile(V2U(0, 5), v2f_add(car_list[i].pos, V2F(-24, -8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        draw_tile(V2U(3, 5), v2f_add(car_list[i].pos, V2F(+24, -8)), V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
        break;
      default:
        assert(false);
        break;
    }
  }
  submit_batch();
}

void
__quit(void) {
}
