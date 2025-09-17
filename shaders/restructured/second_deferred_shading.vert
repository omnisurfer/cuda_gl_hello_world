# version 410 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 texture_coords;

out vec2 tex_coords;

void main()
{
    tex_coords = vertex_normal.xy;
    gl_Position = vec4(vertex_position, 1.0);
};