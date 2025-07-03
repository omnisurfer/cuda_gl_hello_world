# version 410 core

// in vec3 position_eye, normal_eye;

in vec2 texture_coordinates;
uniform sampler2D basic_texture;

out vec4 frag_color;

void main()
{
    vec4 texel = texture(basic_texture, texture_coordinates);
    frag_color = texel;
    
    // frag_color = vec4(1.0, 0.0, 1.0, 1.0);
}