#version 460 core

out vec4 frag_color;

in vec2 tex_coord;

uniform sampler2D texture0;

void main() {
    frag_color = texture(texture0, tex_coord);
}

