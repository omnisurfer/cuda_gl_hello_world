# version 410 core

in vec2 texture_coords;

out vec4 frag_color;

uniform sampler2D g_buffer_position;
uniform sampler2D g_buffer_normal;
uniform sampler2D g_buffer_albedo_spec;

void main()
{
    // frag_color = vec4(gl_FragCoord.x / 1920.0, gl_FragCoord.y / 1080.0, 0.0, 1.0);
    
    vec3 frag_pos = texture(g_buffer_position, texture_coords).rgb;
    vec3 normal = texture(g_buffer_normal, texture_coords).rgb;
    vec3 albedo = texture(g_buffer_normal, texture_coords).rgb;
    
    frag_color = vec4(normal, 1.0);
}