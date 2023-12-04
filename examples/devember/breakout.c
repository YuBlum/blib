#include <blib.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
  f32 top,
      left,
      right,
      bottom;
} collider;

typedef struct {
  v2f pos;
  collider col;
} block;

static collider screen;

static v2f pad;
static collider pad_collider;

static v2f ball_dir;
static v2f ball;
static collider ball_collider;

static block *blocks;

#define PAD_SPEED 400
#define PAD_SIZE  V2F(64, 16)

#define BALL_SPEED 200
#define BALL_SIZE  V2F(16, 16)

#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 480

b8 game_over;

static collider
collider_update(collider col, v2f pos, v2f siz) {
  col.top    = pos.y + siz.y * 0.5f;
  col.bottom = pos.y - siz.y * 0.5f;

  col.right = pos.x + siz.x * 0.5f;
  col.left  = pos.x - siz.x * 0.5f;
  return col;
}

static void
game_start(void) {
  pad = V2F(0, -WINDOW_HEIGHT * 0.25f);
  ball = V2F(0, 0);

  ball_dir.x = rand() % 2 == 0 ? -1 : 1;
  ball_dir.y = rand() % 2 == 0 ? -1 : 1;

  game_over = false;

  array_list_clear(blocks);

  for (u32 j = 0; j < 5; j++) {
    for (u32 i = 0; i < 7; i++) {
      block b = {0};
      b.pos =
        V2F(
            -WINDOW_WIDTH * 0.5f + PAD_SIZE.x * 0.5f + i * PAD_SIZE.x * 1.5f,
            WINDOW_HEIGHT * 0.4f - PAD_SIZE.y * 0.5f - j * PAD_SIZE.y * 2);
      b.col = collider_update(b.col, b.pos, PAD_SIZE);

      array_list_push(blocks, b);
    }
  }
}

static b8
collided(collider col, v2f ball_vel) {
  return ball_collider.top    + ball_vel.y > col.bottom &&
         ball_collider.bottom + ball_vel.y < col.top    &&
         ball_collider.right  + ball_vel.x > col.left   &&
         ball_collider.left   + ball_vel.x < col.right;
}

void
__conf(blib_config *config) {
  config->window_title = "BREAKOUT";
}

void
__init(void) {
  srand(time(0));
  screen.top    =  WINDOW_HEIGHT * 0.5f;
  screen.right  =  WINDOW_WIDTH  * 0.5f;
  screen.left   = -WINDOW_WIDTH  * 0.5f;
  screen.bottom = -WINDOW_HEIGHT * 0.5f;
  blocks = array_list_create(sizeof (block));
  game_start();
  game_over = true;
}

void
__loop(f32 dt) {
  if (key_press('Q')) {
    close_window();
  }

  if (game_over) {
    if (key_press(' ')) {
      game_start();
    } else {
      return;
    }
  }

  f32 speed_x = (key_press('D') - key_press('A')) * PAD_SPEED * dt;
  if (pad_collider.right + speed_x < WINDOW_WIDTH * 0.5f
   && pad_collider.left  + speed_x > -WINDOW_WIDTH * 0.5f) {
    pad.x += speed_x;
  }
  pad_collider = collider_update(pad_collider, pad, PAD_SIZE);

  v2f ball_vel = v2f_mul_scalar(ball_dir, BALL_SPEED * dt);
  ball_collider = collider_update(ball_collider, ball, BALL_SIZE);
  if (ball_collider.left  + ball_vel.x < screen.left || 
      ball_collider.right + ball_vel.x > screen.right ) {
    ball_vel.x *= -1;
    ball_dir.x *= -1;
  }
  if ( ball_collider.top    + ball_vel.y > screen.top  ) {
    ball_vel.y *= -1;
    ball_dir.y *= -1;
  }

  if (ball_collider.bottom + ball_vel.y < screen.bottom ) {
    game_over = true;
    return;
  }

  if (ball_collider.bottom + ball_vel.y < pad_collider.top &&
      ball_collider.top + ball_vel.y > pad_collider.top &&
      ball_collider.right > pad_collider.left &&
      ball_collider.left  < pad_collider.right) {
    ball_vel.y *= -1;
    ball_dir.y *= -1;
  }

  for (u32 i = array_list_size(blocks); i < (u32)-1; i--) {
    if (collided(blocks[i].col, ball_vel)) {
      ball_vel.y *= -1;
      ball_dir.y *= -1;
      array_list_remove(blocks, i, 0);
    }
  }

  ball = v2f_add(ball, ball_vel);
  ball_collider = collider_update(ball_collider, ball, BALL_SIZE);
}

void
__draw(void) {
  clear_screen(COL_BLACK);
  draw_quad(pad, PAD_SIZE, COL_YELLOW, 0);
  for (u32 i = 0; i < array_list_size(blocks); i++) {
    draw_quad( blocks[i].pos, PAD_SIZE, COL_BLUE, 0);
  }
  draw_quad(ball, BALL_SIZE, COL_RED, 0);
  submit_batch();
}

void
__quit(void) {
}
