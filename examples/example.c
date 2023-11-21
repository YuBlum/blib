#include <stdio.h>
#include <blib.h>
#include <stdlib.h>
#include <uuid/uuid.h>

void
__config(blib_config *config) {
  entity_type_begin(STR("enemy"));
  entity_type_add_component(STR("position"), sizeof (v2));
  entity_type_add_component(STR("velocity"), sizeof (v2));
  entity_type_end();
  entity zombie = entity_create(STR("enemy"));
  entity vamp   = entity_create(STR("enemy"));
  entity spider = entity_create(STR("enemy"));
  v2 *pos, *vel;
  pos = entity_get_component(zombie, STR("position"));
  vel = entity_get_component(zombie, STR("velocity"));
  pos->x = 69; pos->y = 420; vel->x = 0;  vel->y = -1;
  pos = entity_get_component(vamp, STR("position"));
  vel = entity_get_component(vamp, STR("velocity"));
  pos->x = 123; pos->y = 321; vel->x = 1;   vel->y = 1;
  pos = entity_get_component(spider, STR("position"));
  vel = entity_get_component(spider, STR("velocity"));
  pos->x = 320; pos->y = 640; vel->x = 2;   vel->y = 0;
  v2 *positions  = entity_type_get_components(STR("enemy"), STR("position"));
  v2 *velocities = entity_type_get_components(STR("enemy"), STR("velocity"));
  for (u32 i = 0; i < array_list_size(positions); i++) {
    printf("before motion position[%u] = { x: %.2f, y: %.2f }\n", i, positions[i].x, positions[i].y);
    positions[i].x += velocities[i].x; positions[i].y += velocities[i].y;
    printf("after  motion position[%u] = { x: %.2f, y: %.2f }\n\n", i, positions[i].x, positions[i].y);
  }
  exit(0);
}

void
__scenes(void *scenes, u32 scenes_amount) {
}

