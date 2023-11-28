#version 330 core

out vec4 color;
in vec4 v_blend;
in vec2 v_texcoord;

uniform sampler2D tex;

void
main() {
  color = texture(tex, v_texcoord) * v_blend;
}

