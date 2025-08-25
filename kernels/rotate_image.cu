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

__device__ void print_thread_details(int block_start_index, int thread_start_index) {
	
	printf("blockIdx.x %i threadId.x %i block_start_index %i thread_startIdx %i\n",
		blockIdx.x,
		threadIdx.x,
		block_start_index,
		thread_start_index
	);

	/*
	printf("thread-blockIdx.xy %i %i %i %i index.ij %i %i blockDim.xy %i %i stride-device_pitch %i %i\n",
		threadIdx.x, threadIdx.y,
		blockIdx.x, blockIdx.y,
		index_i, index_j,
		block_dim_x, block_dim_y,
		stride, device_pitch
	);
	*/
}

__device__ void update_memory_location(int row_index, int column_index) {

}

__global__ void rotateImageKernel(
	float* input_image_data, 
	float* output_image_data,
	int x_dimension,
	int y_dimension,
	int stride,
	int block_size,
	int compute_pitch,
	int device_pitch,
	float angle_radians
) {

	int x_center = x_dimension - x_dimension / 2;
	int y_center = y_dimension - y_dimension / 2;

	// printf("dim.xy %i %i stride-block_size-device_pitch %i %i %i\n", x_dimension, y_dimension, stride, block_size, device_pitch);
	if (true) {

		/* 
		 * More flexible kernel that is based on the simpleCUDA2GL.cu kernel example in the CUDA SDK example projects.		
		*/

		int thread_id_x = threadIdx.x;
		int thread_id_y = threadIdx.y;

		int block_width = blockDim.x;
		int block_height = blockDim.y;

		int block_id_x = blockIdx.x;
		int block_id_y = blockIdx.y;

		int pos_x = block_id_x * block_width + thread_id_x;
		int pos_y = block_id_y * block_height + thread_id_y;
		
		int current_row_index = pos_y * x_dimension;
		int current_column_index = pos_x;

		int current_output_index = current_row_index + current_column_index;
		int current_input_index = current_output_index;

		int rotated_input_index = current_input_index;

		float raw_new_column_index = 
			((float)pos_x - x_center) * cos(angle_radians) - 
			((float)pos_y - y_center) * sin(angle_radians) + x_center;
		
		if (raw_new_column_index < 1) {
			raw_new_column_index = 0;
		}

		int new_column_index = int(round(raw_new_column_index));
		
		// clamp new_column_index
		if (new_column_index > x_dimension - 1) {
			new_column_index = x_dimension - 1;
		}		

		float raw_new_row_index = 
			((float)pos_x - x_center) * sin(angle_radians) + 
			((float)pos_y - y_center) * cos(angle_radians) + y_center;
		
		if (raw_new_row_index < 1) {
			raw_new_row_index = 0;
		}

		int new_row_index = int(round(raw_new_row_index));

		// clamp new_row_index
		if (new_row_index > y_dimension - 1) {
			new_row_index = y_dimension - 1;
		}

		if (new_column_index >= 0 && new_column_index < y_dimension) {
			
			int input_new_row_index = new_row_index * x_dimension;
			int input_new_column_index = new_column_index;

			int current_input_index = input_new_row_index + input_new_column_index;

			output_image_data[current_output_index] = input_image_data[current_input_index];
		}
	}
	else if (false) {

		/*
		* Better way that actually parallelizes the rotation work... BUT seems to only work when explicitly "tuned" for 2048 x 2048 images
		* x_dim, y_dim 2048 2048
		* dim.xy 2048 2048 stride-block_size-device_pitch 4 8 16
		*/

		// device_pitch = thread per block		
		int index_x_addressed_memory_per_block = x_dimension / block_size; // 2048/8=256 2048/64=32
		int index_x_addressed_memory_per_thread = index_x_addressed_memory_per_block / compute_pitch; // 256/16=16 32/16=2

		int index_x_block_pitch_offset = blockIdx.x * index_x_addressed_memory_per_block;
		int index_x_thread_pitch_offset = threadIdx.x * index_x_addressed_memory_per_thread;

		// row processing		
		int thread_start_index = index_x_block_pitch_offset + index_x_thread_pitch_offset;
			
		// print_thread_details(0, thread_start_index);

		// loop through this threads memory section		
		// y_dimension = 10;
		for (int row_index = 0; row_index < y_dimension; ++row_index) {
							
			// printf("column-threadStrartIdx: %i %i\n", thread_start_index, index_x_addressed_memory_per_thread);
			for (int column_index = thread_start_index; column_index < thread_start_index + index_x_addressed_memory_per_thread; ++column_index) {
					
				/*
				printf("blockIdx.x %i threadIdx.x %i row-Idx %i memory/column_index %i thread_start_index %i\n", 
					blockIdx.x, 
					threadIdx.x,
					row_index,
					column_index,
					thread_start_index
				);
				*/

				float* output_row_ptr = (float*)((char*)output_image_data + row_index * device_pitch);

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
				float* new_input_row_ptr = (float*)((char*)input_image_data + new_row_index * device_pitch);
					
				// printf("A writing too r/c %i %i y_dim %i\n", column_index, new_column_index, y_dimension);

				if (new_column_index >= 0 && new_column_index < y_dimension) {							

					// printf("B writing too r/c %i %i\n", column_index, new_column_index);

					output_row_ptr[column_index] = new_input_row_ptr[new_column_index];
				}
			}					
		}
	}			
	else {

		/*
		* OLD Loop based rotation. Does work, but very slow since it basically just runs on a single core it seems
		* - reference: https://stackoverflow.com/questions/9833316/cuda-image-rotation
		*/

		for (int row_index = 0; row_index < y_dimension; ++row_index) {

			for (int column_index = 0; column_index < x_dimension; ++column_index) {

				/* unmodified row pointers output_row_ptr indexes unmodified */
				float* input_row_ptr = (float*)((char*)input_image_data + row_index * device_pitch);
				float* output_row_ptr = (float*)((char*)output_image_data + row_index * device_pitch);

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
				float* new_input_row_ptr = (float*)((char*)input_image_data + new_row_index * device_pitch);

				if (true) {

					if (new_column_index >= 0 && new_column_index < y_dimension) {

						/*
						if (false) {
							printf("orig x,y %i %i\tnew x,y %i %i\tnew raw %f %f\n",
								row_index,
								column_index,
								new_row_index,
								new_column_index,
								raw_new_row_index,
								raw_new_column_index
							);
						}
						*/

						output_row_ptr[column_index] = new_input_row_ptr[new_column_index];
					}
				}				
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
	
	int block_size = 0; 
	size_t compute_pitch; 

	// launch kernel - working, not optimal. Only works with 2048x2048 images.
	if (false) {

		block_size = 64; //8; //86 for RTX 3090
		compute_pitch = device_pitch; // 8192 from getpitch() call, i.e. threads per block
		compute_pitch = 32;

		// https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#execution-configuration
		printf("launching with block, stride, c-d-h_pitch %i-b %i-s %i-c %i-d %i-h\n",
			block_size, stride, compute_pitch, device_pitch, host_pitch
		);
		printf("x_dim, y_dim %i %i\n", x_dimension, y_dimension);

		printf("block size %i, compute_pitch %i", block_size, compute_pitch);
		rotateImageKernel <<<block_size, compute_pitch>>> (
			device_input_buffer,
			device_output_buffer,
			x_dimension,
			y_dimension,
			stride,
			block_size,
			compute_pitch,
			device_pitch,
			angle_radians
			);
	}	
	else if (true) {
		
		// taken from simpleCUDA2GL example kernel
		
		//32 * 32 = 1024 threads per block
		// dim3 block(32, 32, 1);
		dim3 block(32, 32, 1);

		// would need to rework kernel to match this paradiagm
		block_size = block.x + block.y;

		/* 
		2048 / 32 = 64 - 64 * 64 = 4096, the number of grids containing the blocks needed to get the job done, the GPU will basically manage
		the grids as it feeds them to the GPU to be processed as a group at the end of execution.
		*/

		// x_dimension = 128;
		// y_dimension = x_dimension;

		dim3 grid(x_dimension / block.x, y_dimension / block.y, 1);
		compute_pitch = block.x;

		printf("block %i grid %i\n", block.x, grid.x);
		printf("t_id_x, t_id_y, bk_id_x, bk_id_y, bk_w, bk_h, pos_x, pos_y\n");
		rotateImageKernel<<<grid, block, 0>>> (
			device_input_buffer,
			device_output_buffer,
			x_dimension,
			y_dimension,
			stride,
			block_size,
			compute_pitch,
			device_pitch,
			angle_radians
			);
	}
	else {
		// do nothing...
	}

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
		fprintf(stderr, "cudaMemcpy2D failed: device to host!\n");
		goto Error;
	}

Error:
	cudaFree(device_input_buffer);
	cudaFree(device_output_buffer);

	return cuda_status;
}