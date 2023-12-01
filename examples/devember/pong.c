#include <stdio.h>
#include <blib.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
  f32 right,
      left,
      top,
      bot;
} collider;

#define WINDOW_W 640
#define WINDOW_H 480

#define PAD_SPEED   300
#define PAD_SIZE    V2F(8, 48)
#define PAD_PADDING 64

#define PAD_1_START_POS V2F(-WINDOW_W * 0.5f + PAD_SIZE.x * 0.5f + PAD_PADDING, 0)
#define PAD_2_START_POS V2F(WINDOW_W * 0.5f - PAD_SIZE.x * 0.5f - PAD_PADDING, 0)

#define BALL_SIZE  V2F(8, 8)
#define BALL_SPEED 300

#define LINE_SIZE      V2F(2, 10)
#define LINE_PADDING_Y 6

static v2f pad_1;
static v2f pad_2;

static u32 pad_1_points;
static u32 pad_2_points;

static v2f pad_1_points_pos;
static v2f pad_2_points_pos;

static v2f ball_pos;
static v2f ball_dir;
static v2f ball_vel;

static b8 game_start;

static b8
collided(collider c1, collider c2) {
  return c1.right > c2.left  &&
         c1.left  < c2.right &&
         c1.top   > c2.bot   &&
         c1.bot   < c2.top;
}

static v2f
move_pad(v2f pad, u32 up, u32 down, f32 dt) {
  f32 y_vel = (key_press(up) - key_press(down)) * PAD_SPEED * dt;
  if ((pad.y + PAD_SIZE.y * 0.5f + y_vel < WINDOW_H * 0.5f) &&
      (pad.y - PAD_SIZE.y * 0.5f + y_vel > -WINDOW_H * 0.5f)) {
    pad.y += y_vel;
  }
  return pad;
}

static void
ball_start(void) {
  ball_pos = V2F_0;
  ball_dir.x = (rand() % 2) == 0 ? -1 : 1;
  ball_dir.y = (rand() % 2) == 0 ? -1 : 1;
}

static void
ball_collide_with_pad(v2f pad) {
  collider pad_left = {
    pad.x - PAD_SIZE.x * 0.5f,
    pad.x - PAD_SIZE.x * 0.5f,
    pad.y + PAD_SIZE.y * 0.5f,
    pad.y - PAD_SIZE.y * 0.5f
  };
  collider pad_right = {
    pad.x + PAD_SIZE.x * 0.5f,
    pad.x + PAD_SIZE.x * 0.5f,
    pad.y + PAD_SIZE.y * 0.5f,
    pad.y - PAD_SIZE.y * 0.5f
  };
  collider pad_top = {
    pad.x + PAD_SIZE.x * 0.5f,
    pad.x - PAD_SIZE.x * 0.5f,
    pad.y + PAD_SIZE.y * 0.5f,
    pad.y + PAD_SIZE.y * 0.5f
  };
  collider pad_bot = {
    pad.x + PAD_SIZE.x * 0.5f,
    pad.x - PAD_SIZE.x * 0.5f,
    pad.y - PAD_SIZE.y * 0.5f,
    pad.y - PAD_SIZE.y * 0.5f
  };

  collider ball_col = {
    ball_pos.x + BALL_SIZE.x * 0.5f,
    ball_pos.x - BALL_SIZE.x * 0.5f,
    ball_pos.y + BALL_SIZE.y * 0.5f,
    ball_pos.y - BALL_SIZE.y * 0.5f
  };

  if (collided(ball_col, pad_left) || collided(ball_col, pad_right)) {
    ball_dir.x *= -1;
    ball_pos.x -= ball_vel.x;
  }
  if (collided(ball_col, pad_bot) || collided(ball_col, pad_top)) {
    ball_dir.y *= -1;
    ball_pos.y -= ball_vel.y;
  }
}

void
__conf(blib_config *config) {
  config->window_title = "PONG";
}

void
__init(void) {
  srand(time(0));
  pad_1 = PAD_1_START_POS;
  pad_2 = PAD_2_START_POS;

  pad_1_points = 0;
  pad_2_points = 0;

  pad_1_points_pos = V2F(-WINDOW_W * 0.16f, WINDOW_H * 0.32f);
  pad_2_points_pos = V2F( WINDOW_W * 0.16f, WINDOW_H * 0.32f);

  game_start = false;

  ball_start();
}

void
__loop(f32 dt) {
  if (key_press('Q')) {
    close_window();
  }
  if (key_press(' ')) {
    game_start = true;
  }
  if (game_start) {
    pad_1 = move_pad(pad_1, 'W', 'S', dt);
    pad_2 = move_pad(pad_2, KEY_UP, KEY_DOWN, dt);


    ball_vel = v2f_mul_scalar(ball_dir, BALL_SPEED * dt);
    ball_pos = v2f_add(ball_pos, ball_vel);

    if ((ball_pos.y + BALL_SIZE.y * 0.5f > WINDOW_H * 0.5f) ||
        (ball_pos.y - BALL_SIZE.y * 0.5f < -WINDOW_H * 0.5f)) {
      ball_dir.y *= -1;
      ball_pos.y -= ball_vel.y;
    }

    ball_collide_with_pad(pad_1);
    ball_collide_with_pad(pad_2);

    if (ball_pos.x > WINDOW_W * 0.5f) {
      pad_1 = PAD_1_START_POS;
      pad_2 = PAD_2_START_POS;
      ball_start();
      game_start = false;
      pad_1_points++;
    }

    if (ball_pos.x < -WINDOW_W * 0.5f) {
      pad_1 = PAD_1_START_POS;
      pad_2 = PAD_2_START_POS;
      ball_start();
      game_start = false;
      pad_2_points++;
    }
  }
}

void
__draw(void) {
  clear_screen(COL_BLACK);
  draw_quad(pad_1, PAD_SIZE, COL_WHITE, 0);
  draw_quad(pad_2, PAD_SIZE, COL_WHITE, 0);
  draw_quad(ball_pos, BALL_SIZE, COL_WHITE, 0);

  v2f line_pos = V2F(0, WINDOW_H * 0.5f - LINE_SIZE.y * 0.5f);
  for (u32 i = 0; i < WINDOW_H / (LINE_SIZE.y + LINE_PADDING_Y); i++) {
    draw_quad(line_pos, LINE_SIZE, COL_WHITE, 0);
    line_pos.y -= LINE_SIZE.y + LINE_PADDING_Y;
  }

  draw_text(pad_1_points_pos, V2F(10, 10), COL_WHITE, 0, STR("%u"), pad_1_points);
  draw_text(pad_2_points_pos, V2F(10, 10), COL_WHITE, 0, STR("%u"), pad_2_points);

  submit_batch();
}

void
__quit(void) {
}

