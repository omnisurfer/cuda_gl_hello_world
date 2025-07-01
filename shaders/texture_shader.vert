# version 410 core

// texture panel bound attributes
layout(location = 2) in vec3 tex_triangle_points;
layout(location = 3) in vec2 tex_triangle_coords;

out vec2 texture_coordinates;

uniform mat4 model_matrix, view_matrix, projection_matrix;

out vec3 position_eye, normal_eye;

void main() {
	       
    texture_coordinates = tex_triangle_coords;
        	
    gl_Position = projection_matrix * view_matrix * vec4(tex_triangle_points, 1.0);
};