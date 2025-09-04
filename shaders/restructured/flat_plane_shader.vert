# version 410 core

// texture panel bound attributes
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 texture_coords;

// TODO need to calculate tex triangle normal...
uniform mat4 model_matrix, view_matrix, projection_matrix;

out vec2 texture_coordinates;

void main() {
    
    texture_coordinates = texture_coords;
        	        
    // must remove model_matrix to get texure to render. model_matrix seems to be null...
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vertex_position, 1.0);
};