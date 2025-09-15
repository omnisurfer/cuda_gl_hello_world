# version 410 core

// debug
// layout (location = 0) out vec4 frag_color;
layout (location = 0) out vec3 g_buffer_position;
layout (location = 1) out vec3 g_buffer_normal;
layout (location = 2) out vec4 g_buffer_albedo_spec;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

in vec3 position_eye, normal_eye;
in vec2 texture_coordinates;

void main()
{       
    g_buffer_position = position_eye;
    g_buffer_normal = normalize(normal_eye);
    
    g_buffer_albedo_spec.rgb = texture(texture_diffuse1, texture_coordinates).rgb;
    g_buffer_albedo_spec.a = texture(texture_specular1, texture_coordinates).r;
    
    // frag_color = vec4(g_buffer_normal, 1.0);
    // frag_color = g_buffer_albedo_spec;
}