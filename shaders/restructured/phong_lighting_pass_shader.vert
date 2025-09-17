# version 410 core
// Unsure if this is basically accessing the locations 
// setup in the geom shader and demoting vec3 normal to vec2 text coords in LearnOpenGL example...
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 texture_coords;

out vec2 TexCoords;

void main()
{
    TexCoords = vertex_normal.xy;
    gl_Position = vec4(vertex_position, 1.0);
};