#version 330 core

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_texcoord;
layout (location = 2) in vec4 a_blend;
layout (location = 3) in float a_angle;

out vec4 v_blend;
out vec2 v_texcoord;

uniform mat3 u_camera;

void
main() {
  
  mat2 transform;
  transform[0][0] = +cos(a_angle);
  transform[0][1] = -sin(a_angle);
  transform[1][0] = +sin(a_angle);
  transform[1][1] = +cos(a_angle);

  gl_Position = vec4(u_camera * vec3(transform * a_position, 1.0), 1.0);
  v_blend = a_blend;
  v_texcoord = a_texcoord;
}

