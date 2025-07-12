# version 410 core

// texture panel bound attributes
layout(location = 0) in vec3 tex_triangle_points;
layout(location = 1) in vec2 tex_triangle_coords;

// TODO need to calculate tex triangle normal...

uniform mat4 model_matrix, view_matrix, projection_matrix;

out vec2 texture_coordinates;

// out vec3 position_eye, normal_eye;

void main() {
	    
    mat2 m_rotation = mat2(0.0, -1.0, 1.0, 0.0);
    
    vec2 centered_texture_coords = tex_triangle_coords - vec2(0.5, 0.5);
    
    vec2 rotated_texture_coords = m_rotation * centered_texture_coords;
    
    vec2 recentered_texture_coords = rotated_texture_coords + vec2(0.5, 0.5);    
    
    texture_coordinates = recentered_texture_coords;
    
    texture_coordinates = tex_triangle_coords;
        	        
    // must remove model_matrix to get texure to render. model_matrix seems to be null...
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(tex_triangle_points, 1.0);
};