#version 460 core

out vec4 frag_color;

in vec2 tex_coord;

uniform sampler2D texture0;

float circle_sdf(in vec2 pos) {
    return length(pos - vec2(0.5, 0.5)) - 0.5;
}

void main() {
    frag_color = texture(texture0, tex_coord) * smoothstep(0.0, 1.0, -circle_sdf(tex_coord));
}

