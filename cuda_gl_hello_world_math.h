#pragma once

struct mat4 {
	float matrix[16];
};

void translate_matrix(mat4* translate_matrix, float dx, float dy, float dz) {

	translate_matrix->matrix[12] = dx;
	translate_matrix->matrix[13] = dy;
	translate_matrix->matrix[14] = dz;
}

void rotate_matrix(mat4* rotate_matrix, float x_theta, float y_theta, float z_theta) {

	mat4 _z_roate_matrix[] = {
		cos(z_theta), sin(z_theta), 0.0f, 0.0f, // first column - a e i m
		-sin(z_theta), cos(z_theta), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, // fourth column - d h l p
	};
}