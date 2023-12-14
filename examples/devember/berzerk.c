#include <blib.h>

static v2f player;
static u32 player_tile;
static f32 player_anim;
static u32 player_moving;
static v2i player_mov;
static f32 player_scale;
static v2f player_bullet;
static v2f player_bullet_size;
static v2f player_bullet_vel;
static b8  player_shooted;
static v2i player_facing;

#define PLAYER_ANIM_SPEED 10
#define PLAYER_SPEED 50
#define BULLET_SPEED 200

#define GAME_W 320
#define GAME_H 260

#define ENEMIES_AMOUNT 4

v2f enemies             [ENEMIES_AMOUNT];
b8  enemies_exists      [ENEMIES_AMOUNT];
v2i enemies_mov         [ENEMIES_AMOUNT];
v2i enemies_facing      [ENEMIES_AMOUNT];
b8  enemies_shooted     [ENEMIES_AMOUNT];
v2f enemies_bullet      [ENEMIES_AMOUNT];
v2f enemies_bullet_vel  [ENEMIES_AMOUNT];
f32 enemies_timer       [ENEMIES_AMOUNT];
f32 enemies_bullet_timer[ENEMIES_AMOUNT];
f32 enemies_anim        [ENEMIES_AMOUNT];
u32 enemies_tile        [ENEMIES_AMOUNT];

#define ENEMIES_SPEED 25
#define ENEMIES_CHANGE_DIR_SPEED 0.5f
#define ENEMIES_SHOOT_SPEED 0.5f

void
__conf(blib_config *config) {
  config->window_title = "BERZERK";
  config->game_width   = GAME_W;
  config->game_height  = GAME_H;
  config->game_scale   = 2;
}

static void
game_start(void) {
  player = V2F_0;
  player_tile = 0;
  player_scale = 1;
  player_facing = V2I(1, 0);
  player_shooted = false;

  for (u32 i = 0; i < ENEMIES_AMOUNT; i++) {
    enemies[i] = V2F(
      (RAND_1() * (GAME_W -  8)) - ((GAME_W -  8) * 0.5f),
      (RAND_1() * (GAME_H - 16)) - ((GAME_H - 16) * 0.5f)
    );

    enemies_exists      [i] = true;
    enemies_mov         [i] = V2I((s32)(rand() % 3) - 1, (s32)(rand() % 3) - 1);
    enemies_shooted     [i] = false;
    enemies_timer       [i] = 0;
    enemies_bullet_timer[i] = RAND_1() * ENEMIES_SHOOT_SPEED;
    enemies_tile        [i] = 0;
    if (enemies_mov[i].x == 0 && enemies_mov[i].y == 0) enemies_facing[i] = V2I(1, 0);
    else enemies_facing[i] = enemies_mov[i];
  }
}

void
__init(void) {
  asset_load(ASSET_ATLAS, STR("berzerk"));
  texture_atlas_setup(STR("berzerk"), 8, 16, 0, 0);

  game_start();
}

void
__loop(f32 dt) {
  player_mov  = V2I(
    key_press('D') - key_press('A'),
    key_press('W') - key_press('S')
  );
  v2f player_vel = v2f_mul_s(v2f_unit(V2F(player_mov.x, player_mov.y)), PLAYER_SPEED * dt);
  if (player_mov.x)  player_scale = player_mov.x;
  player_moving = player_mov.x || player_mov.y;

  if (!player_moving) {
    player_tile = 0;
  } else {
    player_facing = player_mov;
    player_anim += dt * PLAYER_ANIM_SPEED;
    if (player_anim > 1.0f) {
      player_anim = 0;
      player_tile = (player_tile + 1) % 2;
    }
  }
  player = v2f_add(player, player_vel);

  if (!player_shooted && key_click(' ')) {
    player_shooted = true;
    player_bullet  = player;
    player_bullet_vel = v2f_mul_s(v2f_unit(V2F(player_facing.x, player_facing.y)), BULLET_SPEED * dt);
  }
  if (player_shooted) {
    player_bullet = v2f_add(player_bullet, player_bullet_vel);
    if (player_bullet.x - 2 > +GAME_W * 0.5f ||
        player_bullet.x + 2 < -GAME_W * 0.5f ||
        player_bullet.y - 2 > +GAME_H * 0.5f ||
        player_bullet.y + 2 < -GAME_H * 0.5f) {
      player_shooted = false;
    }
  }

  for (u32 i = 0; i < ENEMIES_AMOUNT; i++) {
    if (!enemies_exists[i]) continue;
    enemies_timer[i] += dt * ENEMIES_CHANGE_DIR_SPEED;
    if (enemies_timer[i] > 1.0f) {
      enemies_timer[i] = 0.0f;
      enemies_mov[i] = V2I((s32)(rand() % 3) - 1, (s32)(rand() % 3) - 1);
    }
    if (!enemies_shooted[i]) {
      enemies_bullet_timer[i] += dt * ENEMIES_SHOOT_SPEED;
      if (enemies_bullet_timer[i] > 1.0f) {
        enemies_shooted[i]     = true;
        enemies_bullet[i]      = enemies[i];
        enemies_bullet_vel[i]  = v2f_mul_s(v2f_unit(V2F(enemies_facing[i].x, enemies_facing[i].y)), BULLET_SPEED * dt);
      }
    } else {
      enemies_bullet[i] = v2f_add(enemies_bullet[i], enemies_bullet_vel[i]);
      if (enemies_bullet[i].x - 2 > +GAME_W * 0.5f ||
          enemies_bullet[i].x + 2 < -GAME_W * 0.5f ||
          enemies_bullet[i].y - 2 > +GAME_H * 0.5f ||
          enemies_bullet[i].y + 2 < -GAME_H * 0.5f) {
        enemies_shooted[i] = false;
      }
    }
    if (enemies_mov[i].x != 0 || enemies_mov[i].y != 0) {
      enemies_facing[i] = enemies_mov[i];
      enemies_anim[i] += dt * PLAYER_ANIM_SPEED;
      if (enemies_anim[i] > 1.0f) {
        enemies_anim[i] = 0;
        enemies_tile[i] = (enemies_tile[i] + 1) % 7;
      }
    }

    v2f vel = v2f_mul_s(v2f_unit(V2F(enemies_mov[i].x, enemies_mov[i].y)), ENEMIES_SPEED * dt);
    enemies[i] = v2f_add(enemies[i], vel);
    if (enemies[i].x + 4 > +GAME_W * 0.5f ||
        enemies[i].x - 4 < -GAME_W * 0.5f ||
        enemies[i].y + 8 > +GAME_H * 0.5f ||
        enemies[i].y - 8 < -GAME_H * 0.5f) {
      enemies[i] = v2f_sub(enemies[i], player_vel);
      enemies_mov[i] = v2i_mul_s(enemies_mov[i], -1);
    }

    if (player_shooted                         && 
        player_bullet.x + 2 > enemies[i].x - 4 &&
        player_bullet.x - 2 < enemies[i].x + 4 &&
        player_bullet.y + 2 > enemies[i].y - 8 &&
        player_bullet.y - 2 < enemies[i].y + 8) {
      enemies_exists[i] = false;
    }

    if (enemies_shooted[i]                     && 
        enemies_bullet[i].x + 2 > player.x - 4 &&
        enemies_bullet[i].x - 2 < player.x + 4 &&
        enemies_bullet[i].y + 2 > player.y - 8 &&
        enemies_bullet[i].y - 2 < player.y + 8) {
      game_start();
    }
  }
}

void
__tick(void) {
}

void
__draw(batch *batch) {
  batch->atlas = STR("berzerk");
  clear_screen(COL_BLACK);
  draw_tile(V2U(player_moving + player_tile, 0), player, V2F(player_scale, 1), V2F_0, 0, COL_WHITE, 0);
  if (player_shooted) {
    draw_rect(player_bullet, V2F(2, 2), V2F_0, 0, COL_RED, 0);
  }
  for (u32 i = 0; i < ENEMIES_AMOUNT; i++) {
    if (!enemies_exists[i]) continue;
    draw_tile(V2U(3 + enemies_tile[i], 0), enemies[i], V2F_S(1), V2F_0, 0, COL_WHITE, 0);
    if (enemies_shooted[i]) {
      draw_rect(enemies_bullet[i], V2F(2, 2), V2F_0, 0, COL_RED, 0);
    }

  }
  submit_batch();
}

void
__quit(void) {
}
