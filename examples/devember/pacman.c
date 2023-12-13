#include <blib.h>

#define TILE_SIZE 8

#define PACMAN STR("pacman")

#define GAME_W 224
#define GAME_H 288

#define GAME_TOP    (+GAME_H * 0.5f)
#define GAME_RIGHT  (+GAME_W * 0.5f)
#define GAME_LEFT   (-GAME_W * 0.5f)
#define GAME_BOTTOM (-GAME_H * 0.5f)

typedef enum {
  TILE_NONE = 0,
  TILE_OBSTACLE,
  TILE_BIG_COIN,
  TILE_COIN,
  TILE_PACMAN,
  TILE_GHOST
} tile_type;

typedef enum {
  DIR_RIGHT = 0,
  DIR_UP,
  DIR_LEFT,
  DIR_DOWN,
} dir;

static v2f dir_pos[] = {
  { +TILE_SIZE, 0          },
  { 0,          +TILE_SIZE },
  { -TILE_SIZE, 0          },
  { 0,          -TILE_SIZE }
};

/*
 *
 *        <_> [=]
 *        |./ lOi
 *        (-) {+}
 *  #$ ^~ ', 12 56
 *     ;: ?! 34 78
 *
 */

#define MAP_W 28
#define MAP_H 31
static cstr map = 
"^++++++++++++21++++++++++++~"
"i............|/............l"
"i.<__>.<___>.|/.<___>.<__>.l"
"i0|  /.|   /.|/.|   /.|  /0l"
"i.(--).(---).().(---).(--).l"
"i..........................l"
"i.<__>.<>.<______>.<>.<__>.l"
"i.(--).|/.(--,'--).|/.(--).l"
"i......|/....|/....|/......l"
";====>.|?__> |/ <__!/.<====:"
"     i.|'--) () (--,/.l     "
"     i.|/  G G G G |/.l     "
"     i.|/ [======] |/.l     "
"+++++).() l      i ().(+++++"
"      .   l      i   .      "
"=====>.<> l      i <>.<====="
"     i.|/ {++++++} |/.l     "
"     i.|/          |/.l     "
"     i.|/ <______> |/.l     "
"^++++).() (--,'--) ().(++++~"
"i............|/............l"
"i.<__>.<___>.|/.<___>.<__>.l"
"i.(-,/.(---).().(---).|'-).l"
"i0..|/.......C .......|/..0l"
"7_>.|/.<>.<______>.<>.|/.<_8"
"5-).().|/.(--,'--).|/.().(-6"
"i......|/....|/....|/......l"
"i.<____!?__>.|/.<__!?____>.l"
"i.(--------).().(--------).l"
"i..........................l"
";==========================:";

static v2u *obstacles_tiles;
static v2f *obstacles_pos;
static v2f *big_coins;
static v2f *coins;

typedef struct {
  v2f pos;
  v2f prv;
  v2f nxt;
  u32 sprite;
  f32 time;
  b8  moving;
  dir dir;
  dir nxt_dir;
  f32 anim_timer;

  /* pacman only */
  f32 angle;
  f32 is_big;

  /* ghosts only */
  f32 timer;
  b8 exists;
} pac_entity;

static pac_entity pacman;

static pac_entity ghosts[4];

#define PACMAN_ANIM_SPEED 15
#define PACMAN_SPEED 10

static void
game_start(void) {
  array_list_clear(obstacles_pos);
  array_list_clear(obstacles_tiles);
  array_list_clear(big_coins);
  array_list_clear(coins);
  u32 ghost_setup = 0;
  for (u32 i = 0; i < MAP_H; i++) {
    for (u32 j = 0; j < MAP_W; j++) {
      v2f pos = V2F(
        j * TILE_SIZE - GAME_W * 0.5f + TILE_SIZE * 0.5f,
        i * TILE_SIZE - GAME_H * 0.5f + TILE_SIZE * 3
      );
      v2u tile = V2U_0;
      tile_type type = TILE_NONE;
      switch (map[(MAP_H - (i + 1)) * MAP_W + j]) {
        case '<':  tile = V2U(10,  8); type = TILE_OBSTACLE; break;
        case '_':  tile = V2U(11,  8); type = TILE_OBSTACLE; break;
        case '>':  tile = V2U(12,  8); type = TILE_OBSTACLE; break;
        case '|':  tile = V2U(10,  9); type = TILE_OBSTACLE; break;
        case '/':  tile = V2U(12,  9); type = TILE_OBSTACLE; break;
        case '(':  tile = V2U(10, 10); type = TILE_OBSTACLE; break;
        case '-':  tile = V2U(11, 10); type = TILE_OBSTACLE; break;
        case ')':  tile = V2U(12, 10); type = TILE_OBSTACLE; break;

        case '[':  tile = V2U(13,  8); type = TILE_OBSTACLE; break;
        case '=':  tile = V2U(14,  8); type = TILE_OBSTACLE; break;
        case ']':  tile = V2U(15,  8); type = TILE_OBSTACLE; break;
        case 'l':  tile = V2U(13,  9); type = TILE_OBSTACLE; break;
        case 'i':  tile = V2U(15,  9); type = TILE_OBSTACLE; break;
        case '{':  tile = V2U(13, 10); type = TILE_OBSTACLE; break;
        case '+':  tile = V2U(14, 10); type = TILE_OBSTACLE; break;
        case '}':  tile = V2U(15, 10); type = TILE_OBSTACLE; break;

        case '^':  tile = V2U( 8, 11); type = TILE_OBSTACLE; break;
        case '~':  tile = V2U( 9, 11); type = TILE_OBSTACLE; break;
        case ';':  tile = V2U( 8, 12); type = TILE_OBSTACLE; break;
        case ':':  tile = V2U( 9, 12); type = TILE_OBSTACLE; break;

        case '\'': tile = V2U(10, 11); type = TILE_OBSTACLE; break;
        case ',':  tile = V2U(11, 11); type = TILE_OBSTACLE; break;
        case '?':  tile = V2U(10, 12); type = TILE_OBSTACLE; break;
        case '!':  tile = V2U(11, 12); type = TILE_OBSTACLE; break;

        case '1':  tile = V2U(12, 11); type = TILE_OBSTACLE; break;
        case '2':  tile = V2U(13, 11); type = TILE_OBSTACLE; break;
        case '3':  tile = V2U(12, 12); type = TILE_OBSTACLE; break;
        case '4':  tile = V2U(13, 12); type = TILE_OBSTACLE; break;

        case '5':  tile = V2U(14, 11); type = TILE_OBSTACLE; break;
        case '6':  tile = V2U(15, 11); type = TILE_OBSTACLE; break;
        case '7':  tile = V2U(14, 12); type = TILE_OBSTACLE; break;
        case '8':  tile = V2U(15, 12); type = TILE_OBSTACLE; break;

        case '#':  tile = V2U( 8, 10); type = TILE_OBSTACLE; break;
        case '$':  tile = V2U( 9, 10); type = TILE_OBSTACLE; break;

        case '0':  type = TILE_BIG_COIN;                     break;
        case '.':  type = TILE_COIN;                         break;

        case 'C':  type = TILE_PACMAN;                       break;

        case 'G':  type = TILE_GHOST;                        break;
      }
      switch (type) {
        case TILE_OBSTACLE:
          array_list_push(obstacles_tiles, tile);
          array_list_push(obstacles_pos, pos);
          break;
        case TILE_BIG_COIN:
          array_list_push(big_coins, pos);
          break;
        case TILE_COIN:
          array_list_push(coins, pos);
          break;
        case TILE_PACMAN:
          pacman.pos = v2f_add(pos, V2F(TILE_SIZE * 0.5f, 0));
          pacman.nxt = pacman.pos;
          pacman.prv = pacman.pos;
          pacman.is_big = 0;
          break;
        case TILE_GHOST:
          ghosts[ghost_setup].pos = v2f_add(pos, V2F(TILE_SIZE * 0.5f, 0));
          ghosts[ghost_setup].nxt = ghosts[ghost_setup].pos;
          ghosts[ghost_setup].prv = ghosts[ghost_setup].pos;
          ghosts[ghost_setup].dir = rand() % 4;
          ghosts[ghost_setup].nxt_dir = ghosts[ghost_setup].dir;
          ghosts[ghost_setup].timer = RAND_1() * 0.2f;
          ghosts[ghost_setup++].exists = true;
          break;
      };
    }
  }
  pacman.dir = DIR_RIGHT;
  pacman.nxt_dir = pacman.dir;
}

static b8
collided(v2f p1, v2f p2) {
  v2i a = { (s32)p1.x / TILE_SIZE, (s32)p1.y / TILE_SIZE };
  v2i b = { (s32)p2.x / TILE_SIZE, (s32)p2.y / TILE_SIZE };
  return a.x == b.x && a.y == b.y;
}

static s32
collided_list(v2f p, v2f *ps) {
  v2i a = { (s32)p.x / TILE_SIZE, (s32)p.y / TILE_SIZE };
  for (s32 i = 0; i < array_list_size(ps); i++) {
    v2i b = { (s32)ps[i].x / TILE_SIZE, (s32)ps[i].y / TILE_SIZE };
    if (a.x == b.x && a.y == b.y) {
      return i;
    }
  }
  return -1;
}

static void
move_entity(pac_entity *e, f32 dt, f32 speed, f32 anim_speed, u32 frames_amount, b8 is_ghost) {
  if (e->nxt.x + TILE_SIZE > GAME_RIGHT || e->nxt.x - TILE_SIZE < GAME_LEFT) {
    e->nxt_dir = e->dir;
  }
  if (e->nxt.x - TILE_SIZE > GAME_RIGHT) {
    e->moving = true;
    e->nxt.x = GAME_LEFT + TILE_SIZE;
    e->prv.x = GAME_LEFT;
  } else if (e->nxt.x + TILE_SIZE < GAME_LEFT) {
    e->moving = true;
    e->nxt.x = GAME_RIGHT - TILE_SIZE;
    e->prv.x = GAME_RIGHT;
  } else if (!is_ghost) {
    if (key_click('D')) e->nxt_dir = DIR_RIGHT;
    if (key_click('A')) e->nxt_dir = DIR_LEFT;
    if (key_click('W')) e->nxt_dir = DIR_UP;
    if (key_click('S')) e->nxt_dir = DIR_DOWN;
  } else {
    e->timer += dt;
    if (e->timer > 0.2f) {
      e->timer = 0;
      e->nxt_dir = rand() % 4;
    }
  }
  if (!e->moving) {
    if (collided_list(v2f_add(e->nxt, dir_pos[e->nxt_dir]), obstacles_pos) == -1) {
      e->dir = e->nxt_dir;
    } 
    e->nxt = v2f_add(e->nxt, dir_pos[e->dir]);
    if (collided_list(e->nxt, obstacles_pos) != -1) {
      e->nxt = e->pos;
    } else {
      e->moving = true;
    }
  } else  {
    e->anim_timer += dt * anim_speed;
    if (e->anim_timer > 1.0f)  {
      e->anim_timer = 0;
      e->sprite = (e->sprite + 1) % frames_amount;
    }

    e->time += dt * speed;
    if (e->time >= 1.0f) {
      e->time = 0.0f;
      e->prv = e->pos;
      e->pos = e->nxt;
      e->moving = false;
    } else {
      e->pos = v2f_lerp(e->prv, e->nxt, e->time);
    }
  }
}

void
__conf(blib_config *config) {
  config->window_title = "PACMAN";
  config->game_width   = GAME_W;
  config->game_height  = GAME_H;
  config->game_scale   = 2;
}

void
__init(void) {
  asset_load(ASSET_SPRITE_FONT, PACMAN);
  asset_load(ASSET_ATLAS, PACMAN);
  texture_atlas_setup(PACMAN, 8, 8, 0, 0);

  obstacles_tiles = array_list_create(sizeof (v2u));
  obstacles_pos   = array_list_create(sizeof (v2f));
  big_coins       = array_list_create(sizeof (v2f));
  coins           = array_list_create(sizeof (v2f));

  game_start();
}

void
__loop(f32 dt) {
  if (key_click('Q')) close_window();
  move_entity(&pacman, dt, PACMAN_SPEED, PACMAN_ANIM_SPEED, 3, false);
  pacman.angle = pacman.dir * (PI * 0.5f);

  if (pacman.is_big > 0) {
    pacman.is_big -= dt;
  }  
  
  u32 coin_index = collided_list(pacman.nxt, coins);
  if (coin_index != -1) {
    array_list_remove(coins, coin_index, 0);
  }
  
  u32 big_coin_index = collided_list(pacman.nxt, big_coins);
  if (big_coin_index != -1) {
    pacman.is_big = 3.0f;
    array_list_remove(big_coins, big_coin_index, 0);
  }

  for (u32 i = 0; i < 4; i++) {
    if (!ghosts[i].exists) continue;
    if (collided(ghosts[i].pos, pacman.pos)) {
      if (pacman.is_big > 0) ghosts[i].exists = false;
      else game_start();
    }
    move_entity(ghosts + i, dt, PACMAN_SPEED, PACMAN_ANIM_SPEED, 2, true);
  }

  //collider_update(&pacman.col, pacman);
}

void
__tick(void) {
}

void
__draw(batch *batch) {
  batch->font  = PACMAN;
  batch->atlas = PACMAN;
  clear_screen(COL_BLACK);
  
  /* draw background */
  for (u32 i = 0; i < array_list_size(obstacles_tiles); i++) {
    draw_tile(obstacles_tiles[i], obstacles_pos[i], V2F_S(1), V2F_0, 0, COL_WHITE, 0);
  }
  
  for (u32 i = 0; i < array_list_size(big_coins); i++) {
    draw_tile(V2U(11, 9), big_coins[i], V2F_S(1), V2F_0, 0, COL_WHITE, 0);
  }
  
  for (u32 i = 0; i < array_list_size(coins); i++) {
    draw_tile(V2U(14, 9), coins[i], V2F_S(1), V2F_0, 0, COL_WHITE, 0);
  }


  /* draw pacman */
  v2f offset;


  offset = V2F(-TILE_SIZE * 0.5f, -TILE_SIZE * 0.5f);
  draw_tile(V2U(0 + pacman.sprite * 2, 12), v2f_add(pacman.pos, offset), V2F_S(1), offset, pacman.angle, COL_WHITE, 0);

  offset = V2F(+TILE_SIZE * 0.5f, -TILE_SIZE * 0.5f);
  draw_tile(V2U(1 + pacman.sprite * 2, 12), v2f_add(pacman.pos, offset), V2F_S(1), offset, pacman.angle, COL_WHITE, 0);

  offset = V2F(+TILE_SIZE * 0.5f, +TILE_SIZE * 0.5f);
  draw_tile(V2U(1 + pacman.sprite * 2, 11), v2f_add(pacman.pos, offset), V2F_S(1), offset, pacman.angle, COL_WHITE, 0);

  offset = V2F(-TILE_SIZE * 0.5f, +TILE_SIZE * 0.5f);
  draw_tile(V2U(0 + pacman.sprite * 2, 11), v2f_add(pacman.pos, offset), V2F_S(1), offset, pacman.angle, COL_WHITE, 0);

  /* draw ghosts */
  for (u32 i = 0; i < 4; i++) {
    if (!ghosts[i].exists) continue;
    offset = V2F(-TILE_SIZE * 0.5f, -TILE_SIZE * 0.5f);
    f32 color = pacman.is_big > 0 ? 8 : i * 2;
    f32 dir   = pacman.is_big > 0 ? 0 : (ghosts[i].dir * 4);
    draw_tile(V2U(dir + 0 + ghosts[i].sprite * 2, color + 1),
        v2f_add(ghosts[i].pos, offset), V2F_S(1), V2F_0, 0, COL_WHITE, 0);

    offset = V2F(+TILE_SIZE * 0.5f, -TILE_SIZE * 0.5f);
    draw_tile(V2U(dir + 1 + ghosts[i].sprite * 2, color + 1),
        v2f_add(ghosts[i].pos, offset), V2F_S(1), V2F_0, 0, COL_WHITE, 0);

    offset = V2F(+TILE_SIZE * 0.5f, +TILE_SIZE * 0.5f);
    draw_tile(V2U(dir + 1 + ghosts[i].sprite * 2, color + 0),
        v2f_add(ghosts[i].pos, offset), V2F_S(1), V2F_0, 0, COL_WHITE, 0);

    offset = V2F(-TILE_SIZE * 0.5f, +TILE_SIZE * 0.5f);
    draw_tile(V2U(dir + 0 + ghosts[i].sprite * 2, color + 0),
        v2f_add(ghosts[i].pos, offset), V2F_S(1), V2F_0, 0, COL_WHITE, 0);
  }

  submit_batch();
}

void
__quit(void) {
}
