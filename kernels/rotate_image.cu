#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <math.h>

#include <math_constants.h>

cudaError_t rotateImage(
	const float* input_image_data,
	float* output_image_data,
	int x_dimension,
	int y_dimension,
	int stride,
	float angle_degrees	
);

__global__ void rotateImageKernel(
	float* input_image_data, 
	float* output_image_data,
	int x_dimension,
	int y_dimension,
	int stride,
	size_t pitch,
	float angle_radians
) {

	/*
	- reference: https://stackoverflow.com/questions/9833316/cuda-image-rotation
	*/

	if (false) {
		printf("block idx/y %u %u, thread idx/y %u %u, blockDim x/y %u %u\n",
			blockIdx.x,
			blockIdx.y,
			threadIdx.x,
			threadIdx.y,
			blockDim.x,
			blockDim.y
		);
	}
	
	int index_i = blockIdx.x * pitch + threadIdx.x;
	int index_j = blockIdx.y * pitch + threadIdx.y;

	int x_center = x_dimension - x_dimension / 2;
	int y_center = y_dimension - y_dimension / 2;
		
	for (int row_index = 0; row_index < y_dimension; ++row_index) {
								
		for (int column_index = 0; column_index < x_dimension; ++column_index) {

			/* unmodified row pointers output_row_ptr indexes unmodified */
			float* input_row_ptr = (float*)((char*)input_image_data + row_index * pitch);
			float* output_row_ptr = (float*)((char*)output_image_data + row_index * pitch);
			
			float raw_new_column_index = ((float)column_index - x_center) * cos(angle_radians) - ((float)row_index - y_center) * sin(angle_radians) + x_center;
			
			if (raw_new_column_index < 1) {
				raw_new_column_index = 0;
			}
			
			int new_column_index = int(round(raw_new_column_index));

			// clamp new_column_index
			if (new_column_index < 1)
			{
				new_column_index = 0;
			}
			else if (new_column_index > x_dimension - 1)
			{
				new_column_index = x_dimension - 1;
			}

			float raw_new_row_index = ((float)column_index - x_center) * sin(angle_radians) + ((float)row_index - y_center) * cos(angle_radians) + y_center;
			
			if (raw_new_row_index < 1)
			{
				raw_new_row_index = 0;
			}

			int new_row_index = int(round(raw_new_row_index));

			// clamp new_row_index
			if (new_row_index < 1)
			{
				new_row_index = 0;
			}
			else if (new_row_index > y_dimension - 1)
			{
				new_row_index = y_dimension - 1;
			}

			/* input_row_ptr must point to where I want to read a pixel from after transform */
			float* new_input_row_ptr = (float*)((char*)input_image_data + new_row_index * pitch);

			if (true) {
				
				if (new_column_index >= 0 && new_column_index < y_dimension) {
					
					/**/
					printf("orig x,y %i %i\tnew x,y %i %i\tnew raw %f %f\n", 
						row_index, column_index, new_row_index, new_column_index, raw_new_row_index, raw_new_column_index);
					/**/

					output_row_ptr[column_index] = new_input_row_ptr[new_column_index];
				}
			}			

			if (false) {
				printf("post idx.x %i idx.y %i out %f in %f\n", 
					index_i, 
					index_j,					
					input_row_ptr[column_index], 
					output_row_ptr[column_index]
				);
			}
		}
	}	
}

extern "C" {

	int execute_image_rotation_kernel(
		const float* input_image_data, 
		float* output_image_data,
		int x_dimension, 
		int y_dimension, 
		int stride, 
		float angle_degrees		
	) {

		cudaError_t cuda_status = rotateImage(
			input_image_data,
			output_image_data,
			x_dimension,
			y_dimension,
			stride,
			angle_degrees	
		);

		if (cuda_status != cudaSuccess) {
			fprintf(stderr, "rotateImage failed to execute!\n");
			return 1;
		}

		// cudaDeviceReset must be called before exiting in order for profiling and
		// tracing tools such as Nsight and Visual Profiler to show complete traces.
		cuda_status = cudaDeviceReset();
		if (cuda_status != cudaSuccess) {
			fprintf(stderr, "cudaDeviceReset failed!\n");
			return 1;
		}

		return 0;
	}
}

cudaError_t rotateImage(
	const float* input_image_data,
	float* output_image_data,
	int x_dimension,
	int y_dimension,
	int stride,
	float angle_degrees
) {
	
	float* device_input_buffer = 0;
	float* device_output_buffer = 0;
	cudaError_t cuda_status;

	float angle_radians = angle_degrees * (CUDART_PI_F / 180.0);

	// choose GPU
	cuda_status = cudaSetDevice(0);
	if (cuda_status != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed! Selected GPU may not be CUDA compatible.\n");
	}

	
	size_t device_pitch;

	/* 
	- allocate pitched gpu buffers for one input image and an output image
	- reference https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html?highlight=cudaMallocPitch#device-memory		
	*/ 
	cuda_status = cudaMallocPitch(
		&device_input_buffer,
		&device_pitch,
		x_dimension * sizeof(float),
		y_dimension
	);

	if (cuda_status != cudaSuccess) {
		fprintf(stderr, "cudaMallocPitch failed!\n");
		goto Error;
	}

	cuda_status = cudaMallocPitch(
		&device_output_buffer,
		&device_pitch,
		x_dimension * sizeof(float),
		y_dimension
	);

	if (cuda_status != cudaSuccess) {
		fprintf(stderr, "cudaMallocPitch failed!\n");
		goto Error;
	}

	/* 
	- copy input image data from host memory to GPU buffer	
	- for reference
		- https://stackoverflow.com/questions/16119943/how-and-when-should-i-use-pitched-pointer-with-the-cuda-api
		- https://stackoverflow.com/questions/16491232/how-do-i-use-cudamemcpy2d-devicetohost

	*/
	size_t host_pitch = x_dimension * sizeof(float);
	size_t x_dimension_in_bytes = host_pitch;

	cuda_status = cudaMemcpy2D(
		device_input_buffer,
		device_pitch,
		input_image_data,
		host_pitch,
		x_dimension_in_bytes,
		y_dimension,
		cudaMemcpyHostToDevice
	);

	if (cuda_status != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy2D failed, host to device: %s\n", cudaGetErrorString(cuda_status));
		goto Error;
	}
	
	int compute_pitch = 1; // device_pitch;

	// launch kernel
	rotateImageKernel<<<1, compute_pitch>>>(
		device_input_buffer, 
		device_output_buffer,
		x_dimension, 
		y_dimension, 
		stride, 
		device_pitch, 
		angle_radians
		);

	// check for errors launching the kernel
	cuda_status = cudaGetLastError();
	if (cuda_status != cudaSuccess) {
		fprintf(stderr, "Kernel launch failed: %s\n", cudaGetErrorString(cuda_status));
		goto Error;
	}

	// copy memory back to local buffer
	cuda_status = cudaMemcpy2D(
		output_image_data,
		host_pitch,
		device_output_buffer,
		device_pitch,
		x_dimension_in_bytes,
		y_dimension,
		cudaMemcpyDeviceToHost
	);

	if (cuda_status != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy2D failed: host to device!\n");
		goto Error;
	}

Error:
	cudaFree(device_input_buffer);
	cudaFree(device_output_buffer);

	return cuda_status;
}