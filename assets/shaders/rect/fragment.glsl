#version 330 core

out vec4 color;
in vec4 v_blend;

void
main() {
  color = v_blend;
}

