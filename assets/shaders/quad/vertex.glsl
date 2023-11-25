#version 330 core

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec4 a_blend;

out vec4 v_blend;

void
main() {
  gl_Position = vec4(a_position, 0, 1);
  v_blend = a_blend;
}

