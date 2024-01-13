#version 460 core

out vec4 frag_color;

in vec3 color_f;
in vec2 tex_coord_f;

// uniform sampler2D texture0;

void main() {
    // frag_color = texture(texture0, tex_coord_f);
    frag_color = vec4(color_f, 1.0);
}

