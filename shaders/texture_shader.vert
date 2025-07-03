# version 410 core

// texture panel bound attributes
layout(location = 0) in vec3 tex_triangle_points;
layout(location = 1) in vec2 tex_triangle_coords;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

out vec2 texture_coordinates;

// out vec3 position_eye, normal_eye;

void main() {
	       
    texture_coordinates = tex_triangle_coords;
        	
    gl_Position = projection_matrix * view_matrix * vec4(tex_triangle_points, 1.0);
};