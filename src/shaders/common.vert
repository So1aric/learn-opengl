#version 460 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 tex_coord_v;
layout (location = 2) in vec3 color_v;

out vec2 tex_coord;
out vec3 color;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    tex_coord = tex_coord_v;
    color = color_v;
}
