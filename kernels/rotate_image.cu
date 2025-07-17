#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>

cudaError_t rotateImage(
	const unsigned char* input_image_data,
	unsigned char* output_image_data,
	int x_dimension,
	int y_dimension,
	int stride,
	float angle_degrees	
);

__global__ void rotateImageKernel(
	unsigned char* input_image_data, 
	unsigned char* output_image_data,
	int x_dimension,
	int y_dimension,
	int stride,
	size_t pitch,
	float angle_radians
) {
	for (int row_index = 0; row_index < y_dimension; ++row_index) {
		unsigned char* input_row_ptr = input_image_data + row_index * pitch;
		unsigned char* output_row_ptr = output_image_data + row_index * pitch;
		for (int column_index = 0; column_index < x_dimension; ++column_index) {
			output_row_ptr[column_index] = input_row_ptr[column_index];
		}
	}	
}

extern "C" {

	int execute_image_rotation_kernel(
		const unsigned char* input_image_data, 
		unsigned char* output_image_data,
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
			fprintf(stderr, "rotateImage failed to execute!");
			return 1;
		}

		// cudaDeviceReset must be called before exiting in order for profiling and
		// tracing tools such as Nsight and Visual Profiler to show complete traces.
		cuda_status = cudaDeviceReset();
		if (cuda_status != cudaSuccess) {
			fprintf(stderr, "cudaDeviceReset failed!");
			return 1;
		}

		return 0;
	}
}

cudaError_t rotateImage(
	const unsigned char* input_image_data,
	unsigned char* output_image_data,
	int x_dimension,
	int y_dimension,
	int stride,
	float angle_degrees
) {
	
	unsigned char* device_input_buffer = 0;
	unsigned char* device_output_buffer = 0;
	cudaError_t cuda_status;

	float angle_radians = angle_degrees * (3.14 / 180.0);

	// choose GPU
	cuda_status = cudaSetDevice(0);
	if (cuda_status != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed! Selected GPU may not be CUDA compatible.");
	}

	
	size_t device_pitch;

	/* 
	- allocate pitched gpu buffers for one input image and an output image
	- reference https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html?highlight=cudaMallocPitch#device-memory		
	*/ 
	cuda_status = cudaMallocPitch(
		&device_input_buffer,
		&device_pitch,
		x_dimension * sizeof(unsigned char) * stride,
		y_dimension
	);

	if (cuda_status != cudaSuccess) {
		fprintf(stderr, "cudaMallocPitch failed!");
		goto Error;
	}

	cuda_status = cudaMallocPitch(
		&device_output_buffer,
		&device_pitch,
		x_dimension * sizeof(unsigned char) * stride,
		y_dimension
	);

	if (cuda_status != cudaSuccess) {
		fprintf(stderr, "cudaMallocPitch failed!");
		goto Error;
	}

	/* 
	- copy input image data from host memory to GPU buffer	
	- for reference
		- https://stackoverflow.com/questions/16119943/how-and-when-should-i-use-pitched-pointer-with-the-cuda-api
		- https://stackoverflow.com/questions/16491232/how-do-i-use-cudamemcpy2d-devicetohost

	*/
	size_t host_pitch = x_dimension * sizeof(unsigned char); // *stride;
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
		fprintf(stderr, "cudaMemcpy2D failed: host to device!");
		goto Error;
	}
		
	// launch kernel
	rotateImageKernel<<<1, device_pitch>>>(
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
		fprintf(stderr, "cudaMemcpy2D failed: host to device!");
		goto Error;
	}

Error:
	cudaFree(device_input_buffer);
	cudaFree(device_output_buffer);

	return cuda_status;
}