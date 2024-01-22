#version 460 core

layout (location = 0) in vec4 vertex;

out vec2 tex_coord;

void main() {
    gl_Position = vec4(vertex.xy, 0.0, 1.0);
    tex_coord = vertex.zw;
}
