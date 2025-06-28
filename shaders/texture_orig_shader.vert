# version 410 core

// sphere bound attributes
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;

// texture panel bound attributes
layout(location = 2) in vec3 tex_triangle_points;
layout(location = 3) in vec2 tex_triangle_coords;

out vec2 texture_coordinates;

uniform mat4 model_matrix, view_matrix, projection_matrix;

out vec3 position_eye, normal_eye;

void main() {
	
    texture_coordinates = tex_triangle_coords;
		
	position_eye = vec3( view_matrix * model_matrix * vec4( vertex_position, 1.0 ) );
	normal_eye = vec3( view_matrix * model_matrix * vec4(vertex_normal, 0.0) );	
	
	gl_Position = projection_matrix * vec4(position_eye, 1.0);
};