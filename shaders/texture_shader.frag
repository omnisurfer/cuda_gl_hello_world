# version 410 core

#define M_PI 3.1415926535897932384626433832795

// in vec3 position_eye, normal_eye;

in vec2 texture_coordinates;
uniform sampler2D basic_texture;

out vec4 frag_color;

void main()
{           
    mat2 m_rotation = mat2(0.0, -1.0, 1.0, 0.0);
    
    vec2 centered_texture_coords = texture_coordinates - vec2(0.5, 0.5);
    
    vec2 rotated_texture_coords = m_rotation * centered_texture_coords;
    
    vec2 recentered_texture_coords = rotated_texture_coords + vec2(0.5, 0.5);
    
    vec4 texel = texture(basic_texture, texture_coordinates);
    // vec4 texel = texture(basic_texture, recentered_texture_coords);
    frag_color = texel;
    
    // frag_color = vec4(1.0, 0.0, 1.0, 1.0);
}