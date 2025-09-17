# version 410 core

layout(location = 0) in vec3 g_buffer_position;
layout(location = 1) in vec3 g_buffer_normal;
layout(location = 2) in vec4 g_buffer_albedo_spec;

out vec3 buffer_position;
out vec3 buffer_normal;
out vec4 buffer_albedo_spec;

out vec2 texture_coords;

void main()
{
    
    texture_coords = g_buffer_normal.xy;
    
    buffer_position = g_buffer_position;
    buffer_normal = g_buffer_normal;
    buffer_albedo_spec = g_buffer_albedo_spec;
    
    gl_Position = vec4(g_buffer_position, 1.0);
};