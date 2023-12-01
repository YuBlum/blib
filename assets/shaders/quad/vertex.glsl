#version 330 core

layout (location = 0) in vec2 a_position;
layout (location = 2) in vec4 a_blend;

out vec4 v_blend;

uniform mat3 u_camera;

void
main() {
  gl_Position = vec4(u_camera * vec3(a_position, 1.0), 1.0);
  v_blend = a_blend;
}

