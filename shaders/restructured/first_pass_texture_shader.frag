# version 410 core

/*
Seeing if these are needed to clear this error:
Uniform block index exceeds the maximum supported uniform buffers. userParam -1
*/
in vec3 position_eye, normal_eye;

in vec2 texture_coordinates;
uniform sampler2D basic_texture;

out vec4 frag_color;

void main()
{                   
    vec4 texel = texture(basic_texture, texture_coordinates);
    
    frag_color = texel;
}