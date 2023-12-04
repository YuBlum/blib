#include <blib.h>
#include <stdio.h>

#define GAME_W 240
#define GAME_H 135
#define GAME_S 4

#define TILE_SIZE V2F(16, 16)


#define GROUND_Y -TILE_SIZE.y * 0.5f

entity dino;
#define DINO_GRAVITY -0.25f
#define DINO_MAX_GRAVITY -10
#define DINO_JUMP_HEIGHT 3.125f

void
__conf(blib_config *config) {
  config->window_title = "T-REX GAME";
  config->window_width  = GAME_W * GAME_S;
  config->window_height = GAME_H * GAME_S;
  config->camera_width  = GAME_W;
  config->camera_height = GAME_H;
}

void
__init(void) {
  asset_load(ASSET_ATLAS, STR("trexgame"));

  entity_type_begin(STR("dino"));
    entity_type_add_component(STR("position"),   sizeof (v2f));
    entity_type_add_component(STR("velocity_y"), sizeof (f32));
    entity_type_add_component(STR("on_ground"),  sizeof (b8));
  entity_type_end();

  entity_type_begin(STR("movable"));
    entity_type_add_component(STR("position"),   sizeof (v2f));
    entity_type_add_component(STR("velocity_x"), sizeof (f32));
    entity_type_add_component(STR("can_hit"),    sizeof (b8));
  entity_type_end();

  entity_create(STR("dino"), &dino);

  v2f *dino_position   = entity_get_component(&dino, STR("position"));
  f32 *dino_velocity_y = entity_get_component(&dino, STR("velocity_y"));

  dino_position->x = -GAME_W * 0.5f + TILE_SIZE.x;
  dino_position->y = -TILE_SIZE.y * 0.5f;
  *dino_velocity_y = 0;
}

void
__loop(f32 dt) {
  if (key_press('Q')) close_window();

  /* Move movables */
  //v2f *positions  = entity_type_get_components(STR("movable"), STR("position"));
  //v2f *velocities = entity_type_get_components(STR("movable"), STR("velocity"));

  //for (u32 i = 0; i < array_list_size(positions); i++) {
  //  positions[i] = add_v2f(position, velocities);
  //}
}

void
__tick(void) {
  /* Update dino */
  v2f *dino_position   = entity_get_component(&dino, STR("position"));
  f32 *dino_velocity_y = entity_get_component(&dino, STR("velocity_y"));
  b8  *dino_on_ground  = entity_get_component(&dino, STR("on_ground"));

  *dino_on_ground = dino_position->y <= GROUND_Y;

  *dino_velocity_y += DINO_GRAVITY;
  if (*dino_velocity_y < DINO_MAX_GRAVITY) *dino_velocity_y = DINO_MAX_GRAVITY;

  if (key_click_tick('W') && *dino_on_ground) *dino_velocity_y = DINO_JUMP_HEIGHT;

  dino_position->y += *dino_velocity_y;
  if (dino_position->y < GROUND_Y) {
    dino_position->y = GROUND_Y;
  }
}

void
__draw(batch *batch) {
  clear_screen(COL_WHITE);

  batch->atlas = STR("trexgame");

  v2f *dino_position = entity_get_component(&dino, STR("position"));
  draw_tile(V2U(0, 0), *dino_position, V2F(1, 1), COL_WHITE, 0);
  draw_quad(V2F(-GAME_W * 0.5f + TILE_SIZE.x * 2, -TILE_SIZE.y * 0.5f), V2F(16, 16), COL_BLACK, 0);
  submit_batch();
}

void
__quit(void) {
}
