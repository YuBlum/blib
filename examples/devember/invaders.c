#include <blib.h>

#define TILE_SIZE 8
#define GAME_W_TILES 27
#define GAME_H_TILES 28

#define GAME_W (GAME_W_TILES * 8)
#define GAME_H (GAME_H_TILES * 8)
#define GAME_S 3

#define GAME_TOP    (+GAME_H * 0.5f)
#define GAME_RIGHT  (+GAME_W * 0.5f)
#define GAME_BOTTOM (-GAME_H * 0.5f)
#define GAME_LEFT   (-GAME_W * 0.5f)

#define PLAYER_SPEED 80
#define BULLET_SPEED 300
#define BULLET_SIZE V2F(1, 4)

static v2f player_position;
static v2f player_bullet;
static b8  player_shooted;
static u32 player_health;

typedef struct {
  v2f position;
  u32 health;
} block;
static block *blocks;

static v2f *invaders;
static u32 left_invader;
static u32 right_invader;
static u32 bottom_invader;
static f32 invaders_timer;
static s32 invaders_dir;
static f32 invaders_speed;
static f32 invaders_shooting_timer;
static b8  invaders_shooted;
static v2f invaders_bullet;
#define INVADERS_ANIM_SPEED 2


static cstr map =
"..........................."
"..........................."
"..I.I.I.I.I.I.I.I.I.I.I.I.."
"..........................."
"..I.I.I.I.I.I.I.I.I.I.I.I.."
"..........................."
"..I.I.I.I.I.I.I.I.I.I.I.I.."
"..........................."
"..I.I.I.I.I.I.I.I.I.I.I.I.."
"..........................."
"..I.I.I.I.I.I.I.I.I.I.I.I.."
"..........................."
"..........................."
"..........................."
"..........................."
"..........................."
"..........................."
"..........................."
"..........................."
"..........................."
"..........................."
"...BBB...BBB...BBB...BBB..."
"...BBB...BBB...BBB...BBB..."
"...B.B...B.B...B.B...B.B..."
"..........................."
".............P............."
"..........................."
"...........................";

static void
game_start(void) {
  player_shooted = false;

  array_list_clear(blocks);
  array_list_clear(invaders);

  invaders_dir = 1;
  invaders_speed = 10;
  player_health = 3;
  invaders_shooting_timer = 0;
  invaders_shooted = false;
  player_shooted = false;

  v2f tile_pos = { GAME_LEFT + TILE_SIZE * 0.5f, GAME_TOP + TILE_SIZE * 0.5f };
  for (u32 my = 0; my < GAME_H_TILES; my++) {
    tile_pos.x = GAME_LEFT + TILE_SIZE * 0.5f;
    for (u32 mx = 0; mx < GAME_W_TILES; mx++) {
      switch (map[my * GAME_W_TILES + mx]) {
        case 'P':
        {
          player_position = tile_pos;
        } break;
        case 'B':
        {
          array_list_push(blocks, ((block) { tile_pos, 4 }));
        } break;
        case 'I':
        {
          if (array_list_size(invaders) > 0) {
            if (tile_pos.x < invaders[left_invader].x)   left_invader   = array_list_size(invaders);
            if (tile_pos.x > invaders[right_invader].x)  right_invader  = array_list_size(invaders);
            if (tile_pos.y < invaders[bottom_invader].y) bottom_invader = array_list_size(invaders);
          }
          array_list_push(invaders, tile_pos);
        } break;
      }
      tile_pos.x += TILE_SIZE;
    }
    tile_pos.y -= TILE_SIZE;
  }
}

static b8
collided(v2f p1, v2f s1, v2f p2, v2f s2) {
  return p1.x + s1.x * 0.5f > p2.x - s2.x * 0.5f &&
         p1.x - s1.x * 0.5f < p2.x + s2.x * 0.5f &&
         p1.y + s1.y * 0.5f > p2.y - s2.y * 0.5f &&
         p1.y - s1.y * 0.5f < p2.y + s2.y * 0.5f;
}

void
__conf(blib_config *config) {
  config->window_title = "SPACE INVADERS";
  config->game_width   = GAME_W;
  config->game_height  = GAME_H;
  config->game_scale   = GAME_S;
}

void
__init(void) {
  asset_load(ASSET_ATLAS, STR("invaders"));
  texture_atlas_setup(STR("invaders"), 8, 8, 0, 0);

  blocks   = array_list_create(sizeof (block));
  invaders = array_list_create(sizeof (v2f));

  game_start();
}

void
__loop(f32 dt) {

  /* Move player */
  s32 player_dir = (s32)key_press('D') - (s32)key_press('A');
  f32 player_vel = dt * PLAYER_SPEED * player_dir;
  if (player_position.x + TILE_SIZE * 0.5f + player_vel < GAME_RIGHT &&
      player_position.x - TILE_SIZE * 0.5f + player_vel > GAME_LEFT) {
    player_position.x += player_vel;
  }

  /* Player shoot */
  if (!player_shooted) {
    if (key_click('W')) {
      player_shooted = true;
      player_bullet.x = player_position.x;
      player_bullet.y = player_position.y + TILE_SIZE;
    }
  } else {
    player_bullet.y += dt * BULLET_SPEED;
    if (player_bullet.y - BULLET_SIZE.y * 0.5f > GAME_TOP) player_shooted = false;
  }

  /* Bullet destroys block */
  for (u32 i = array_list_size(blocks) - 1; i < ((u32)-1); i--) {
    b8 hit_by_player = false;
    b8 hit_by_invader = false;
    if (player_shooted  && collided(blocks[i].position, V2F_S(TILE_SIZE), player_bullet, BULLET_SIZE)) {
      hit_by_player = true;
    }

    if (invaders_shooted && collided(blocks[i].position, V2F_S(TILE_SIZE), invaders_bullet, BULLET_SIZE)) {
      hit_by_invader = true;
    }

    blocks[i].health -= hit_by_player;
    if (blocks[i].health == 0) {
      array_list_remove(blocks, i, 0);
    } else {
      blocks[i].health -= hit_by_invader;
      if (blocks[i].health == 0) {
        array_list_remove(blocks, i, 0);
      }
    }

    if (hit_by_player) player_shooted = false;
    if (hit_by_invader) invaders_shooted = false;
  }

  /* Move invaders */
  invaders_timer += dt;

  f32 invaders_velocity = dt * invaders_speed * invaders_dir;
  if (invaders[left_invader].x  + invaders_velocity - TILE_SIZE < GAME_LEFT ||
      invaders[right_invader].x + invaders_velocity + TILE_SIZE > GAME_RIGHT) {
    invaders_velocity *= -1;
    invaders_dir *= -1;

    for (u32 i = 0; i < array_list_size(invaders); i++) {
      invaders[i].y -= TILE_SIZE * 0.25f;
    }
  }
  for (u32 i = 0; i < array_list_size(invaders); i++) {
    invaders[i].x += invaders_velocity;
  }

  /* Shoot invaders */
  for (u32 i = array_list_size(invaders) - 1; i < (u32)-1 && player_shooted; i--) {
    if (!collided(invaders[i], V2F(TILE_SIZE * 2 - 4, TILE_SIZE), player_bullet, BULLET_SIZE)) {
      continue;
    }
    player_shooted = false;
    array_list_remove(invaders, i, 0);
    left_invader   = 0;
    right_invader  = 0;
    bottom_invader = 0;
    for (u32 j = 0; j < array_list_size(invaders); j++) {
      if (invaders[j].x < invaders[left_invader].x) left_invader = j;
      if (invaders[j].x > invaders[right_invader].x) right_invader = j;
      if (invaders[j].y < invaders[bottom_invader].y) bottom_invader = j;
    }
    invaders_speed += 0.5f;
  }

  /* Invaders shoot */
  if (!invaders_shooted) {
    invaders_shooting_timer += dt * 2;
    if (invaders_shooting_timer > 1.0f) {
      invaders_shooting_timer = 0;
      invaders_bullet = invaders[rand() % array_list_size(invaders)];
      invaders_shooted = true;
    }
  } else {
    invaders_bullet.y -= dt * BULLET_SPEED;
    if (invaders_bullet.y + BULLET_SIZE.y * 0.5f < GAME_BOTTOM) invaders_shooted = false;
  }

  /* Shoot player */
  if (invaders_shooted &&
      collided(player_position, V2F(TILE_SIZE * 2, TILE_SIZE), invaders_bullet, BULLET_SIZE)) {
    invaders_shooted = false;
    player_health--;
    if (player_health == 0) {
      game_start();
    }
  }

  /* Invaders win */
  if (invaders[bottom_invader].y < player_position.y + 8) game_start();
}

void
__tick(void) {
}

void
__draw(batch *batch) {
  batch->atlas = STR("invaders");

  clear_screen(COL_BLACK);
  
  /* Draw blocks */
  for (u32 i = 0; i < array_list_size(blocks); i++) {
    draw_tile(V2U(4 - blocks[i].health, 1), blocks[i].position, V2F(1, 1), V2F_0, 0, COL_WHITE, 0);
  }
  
  /* Draw invaders */
  for (u32 i = 0; i < array_list_size(invaders); i++) {
    draw_tile(V2U(1 + (u32)(invaders_timer * INVADERS_ANIM_SPEED) % 2, 0),
        v2f_add(invaders[i], V2F(-TILE_SIZE*0.5f, 0)), V2F(+1, 1), V2F_0, 0, COL_WHITE, 0);
    draw_tile(V2U(1 + (u32)(invaders_timer * INVADERS_ANIM_SPEED) % 2, 0),
        v2f_add(invaders[i], V2F(+TILE_SIZE*0.5f, 0)), V2F(-1, 1), V2F_0, 0, COL_WHITE, 0);
  }

  if (invaders_shooted)
    draw_rect(invaders_bullet, BULLET_SIZE, V2F_0, 0, COL_RED, 1);

  /* Draw player */
  draw_tile(V2U(0, 0),
      v2f_add(player_position, V2F(-TILE_SIZE*0.5f, 0)), V2F(+1, 1), V2F_0, 0, COL_WHITE, 0);

  draw_tile(V2U(0, 0),
      v2f_add(player_position, V2F(+TILE_SIZE*0.5f, 0)), V2F(-1, 1), V2F_0, 0, COL_WHITE, 0);

  if (player_shooted)
    draw_rect(player_bullet, BULLET_SIZE, V2F_0, 0, COL_WHITE, 0);

  for (u32 i = 0; i < player_health; i++) {
    v2f position = V2F(GAME_LEFT + 12 + i * 24, GAME_BOTTOM + 12);
    draw_tile(V2U(0, 0),
        v2f_add(position, V2F(-TILE_SIZE*0.5f, 0)), V2F(+1, 1), V2F_0, 0, COL_WHITE, 0);
    draw_tile(V2U(0, 0),
        v2f_add(position, V2F(+TILE_SIZE*0.5f, 0)), V2F(-1, 1), V2F_0, 0, COL_WHITE, 0);
  }

  /* submit batch */
  submit_batch();
}

void
__quit(void) {
}

