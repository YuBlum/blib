#include <blib.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  f32 top,
      left,
      bottom,
      right;
} collider;

#define GAME_W 240
#define GAME_H 135
#define GAME_S 4

#define TILE_SIZE V2F(16, 16)

#define GROUND_Y -TILE_SIZE.y * 0.5f

entity dino;
#define DINO_GRAVITY -0.25f
#define DINO_MAX_GRAVITY -10
#define DINO_JUMP_HEIGHT 3.75f
#define DINO_COLLIDER_SIZE V2F(8, 14)

#define GAME_SPEED_INC 0.05f

f32 spawn_acc;
f32 spawn_acc_speed;
#define CLOUD_ACC_SPEED 10

#define CACTUS_COLLIDER_SIZE V2F(6, 16)

f32 game_time;

f32 cacti_speed;

u32 points;
f32 points_acc;

b8 game_over;

#define POINTS_SPEED 7

static void
collider_update(collider *col, v2f pos, v2f siz) {
  col->top    = pos.y + siz.y * 0.5f;
  col->bottom = pos.y - siz.y * 0.5f;

  col->right = pos.x + siz.x * 0.5f;
  col->left  = pos.x - siz.x * 0.5f;
}

static b8
collided(collider *c1, collider *c2) {
  return c1->top    > c2->bottom &&
         c1->bottom < c2->top    &&
         c1->right  > c2->left   &&
         c1->left   < c2->right;
}

void
__conf(blib_config *config) {
  config->window_title = "T-REX GAME";
  config->game_width  = GAME_W;
  config->game_height = GAME_H;
  config->game_scale  = GAME_S;
}

static void
start_game(void) {
  v2f *dino_pos = entity_get_component(&dino, STR("position"));
  dino_pos->x = -GAME_W * 0.5f + TILE_SIZE.x;
  dino_pos->y = -TILE_SIZE.y * 0.5f;
  *(f32 *)entity_get_component(&dino, STR("velocity_y")) = 0;
  *(v2u *)entity_get_component(&dino, STR("tile"))       = V2U(0, 0);
  collider_update(entity_get_component(&dino, STR("collider")), *dino_pos, DINO_COLLIDER_SIZE);

  cacti_speed = 1;
  game_time = 0;
  points = 1;
  points_acc = 0;
  spawn_acc_speed = 1;
  game_over = false;
}

void
__init(void) {
  asset_load(ASSET_ATLAS, STR("trexgame"));

  entity_type_begin(STR("dino"));
    entity_type_add_component(STR("position"),   sizeof (v2f));
    entity_type_add_component(STR("velocity_y"), sizeof (f32));
    entity_type_add_component(STR("on_ground"),  sizeof (b8));
    entity_type_add_component(STR("tile"),       sizeof (v2u));
    entity_type_add_component(STR("collider"),   sizeof (collider));
  entity_type_end();

  entity_type_begin(STR("movable"));
    entity_type_add_component(STR("position"),   sizeof (v2f));
    entity_type_add_component(STR("velocity_x"), sizeof (f32));
    entity_type_add_component(STR("can_hit"),    sizeof (b8));
    entity_type_add_component(STR("tile"),       sizeof (v2u));
    entity_type_add_component(STR("collider"),   sizeof (collider));
  entity_type_end();

  entity_create(STR("dino"), &dino);
  start_game();
}

void
__loop(f32 dt) {
  if (key_press('Q')) close_window();
  game_time += dt;

  if (game_over) {
    if (key_click(' ')) {
      entity_type_clear(STR("movable"));
      start_game();
    }
    return;
  }

  points_acc += POINTS_SPEED * dt;
  if (points_acc > 1.0f) {
    points_acc = 0;
    points++;
  }

  cacti_speed += GAME_SPEED_INC * dt;

  /* Create movables */
  spawn_acc_speed += GAME_SPEED_INC * dt;
  spawn_acc += spawn_acc_speed * dt;
  if (spawn_acc >= 1.0f) {
    u32 spawn = rand() % 2;
    spawn_acc = 0;
    switch (spawn) {
      case 0:
        for (u32 i = 0; i < 1 + rand() % 2; i++) {
          entity cloud;
          entity_create(STR("movable"), &cloud);
          *(v2f *)entity_get_component(&cloud, STR("position")) = V2F(
              GAME_W * 0.5f + (rand() % (u32)(GAME_W * 0.5f)),
              (rand() % (u32)(GAME_H * 0.4f))
          );
          *(f32 *)entity_get_component(&cloud, STR("velocity_x")) = 1 + rand() % 2;
          *(b8  *)entity_get_component(&cloud, STR("can_hit"))    = false;
          *(v2u *)entity_get_component(&cloud, STR("tile"))       = V2U(0, 7);
        }
        break;
      case 1:
      {
          entity cactus;
          entity_create(STR("movable"), &cactus);
          v2f *pos = entity_get_component(&cactus, STR("position"));
          pos->x = GAME_W * 0.5f + TILE_SIZE.x;
          pos->y = -TILE_SIZE.y * 0.5f;
          *(f32 *)entity_get_component(&cactus, STR("velocity_x")) = cacti_speed;
          *(b8  *)entity_get_component(&cactus, STR("can_hit"))    = true;
          *(v2u *)entity_get_component(&cactus, STR("tile"))       = V2U(0, 5 + (rand() % 2));
          collider_update(entity_get_component(&cactus, STR("collider")),
              *pos, CACTUS_COLLIDER_SIZE);
      } break;
    }
  }

  /* Update dino tile */
  v2u *dino_tile       = entity_get_component(&dino, STR("tile"));
  b8  *dino_on_ground  = entity_get_component(&dino, STR("on_ground"));
  if (*dino_on_ground) {
    dino_tile->y = 1 + (u32)(game_time * 8) % 2;
  } else {
    dino_tile->y = 0;
  }

  /* Update dino collider */
  v2f *dino_pos = entity_get_component(&dino, STR("position"));
  collider *dino_col = entity_get_component(&dino, STR("collider"));
  collider_update(dino_col, *dino_pos, DINO_COLLIDER_SIZE);

  /* Update movable colliders */
  v2f      *mov_positions = entity_type_get_components(STR("movable"), STR("position"));
  b8       *mov_can_hits  = entity_type_get_components(STR("movable"), STR("can_hit"));
  collider *mov_colliders = entity_type_get_components(STR("movable"), STR("collider"));

  for (u32 i = 0; i < array_list_size(mov_positions); i++) {
    if (!mov_can_hits[i]) continue;
    collider_update(&mov_colliders[i], mov_positions[i], CACTUS_COLLIDER_SIZE);
  }

  /* Dino die */
  for (u32 i = 0; i < array_list_size(mov_colliders); i++) {
    if (!mov_can_hits[i]) continue;
    if (collided(dino_col, &mov_colliders[i])) game_over = true;
  }
}

void
__tick(void) {
  if (game_over) return;

  /* Update dino */
  v2f *dino_position   = entity_get_component(&dino, STR("position"));
  f32 *dino_velocity_y = entity_get_component(&dino, STR("velocity_y"));
  b8  *dino_on_ground  = entity_get_component(&dino, STR("on_ground"));

  *dino_on_ground = dino_position->y <= GROUND_Y;

  *dino_velocity_y += DINO_GRAVITY;
  if (*dino_velocity_y < DINO_MAX_GRAVITY) *dino_velocity_y = DINO_MAX_GRAVITY;

  if (key_click_tick(' ') && *dino_on_ground) *dino_velocity_y = DINO_JUMP_HEIGHT;

  dino_position->y += *dino_velocity_y;
  if (dino_position->y < GROUND_Y) {
    dino_position->y = GROUND_Y;
  }

  /* Move movables */
  v2f *mov_positions    = entity_type_get_components(STR("movable"), STR("position"));
  f32 *mov_velocities_x = entity_type_get_components(STR("movable"), STR("velocity_x"));

  for (u32 i = 0; i < array_list_size(mov_positions); i++) {
    mov_positions[i].x -= mov_velocities_x[i];
  }

  printf("movables = %u\n", array_list_size(mov_positions));
}

void
__draw(batch *batch) {
  clear_screen(COL_WHITE);

  batch->atlas = STR("trexgame");

  /* Draw Dino */
  v2f *dino_position = entity_get_component(&dino, STR("position"));
  v2u *dino_tile     = entity_get_component(&dino, STR("tile"));
  draw_tile(*dino_tile, *dino_position, V2F(1, 1), V2F_0, 0, COL_WHITE, 0);

  draw_rect(V2F(0, -TILE_SIZE.y), V2F(GAME_W, 1),
      V2F_0, 0,
      V4F(0.27f, 0.15f, 0.23f, 1.00f), 0);
  for (u32 i = 0; i < GAME_W / TILE_SIZE.x; i++) {
  }

  /* Draw movables */
  v2f *mov_positions  = entity_type_get_components(STR("movable"), STR("position"));
  v2u *mov_tiles      = entity_type_get_components(STR("movable"), STR("tile"));

  for (u32 i = 0; i < array_list_size(mov_positions); i++) {
    draw_tile(mov_tiles[i], mov_positions[i], V2F(1, 1), V2F_0, 0, COL_WHITE, 0); 
  }

  draw_text(V2F(GAME_W * 0.5f - 50, GAME_H * 0.5f - 5), V2F(0.5f, 0.5f), COL_BLACK, 0,
      STR("%.5u"), points);
  submit_batch();
}

void
__quit(void) {
}
