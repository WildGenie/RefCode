#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <CL/cl.h>

#define PROGRAM_FILE "/home/share/basic/5_program/1_kernel_search/test.cl"

#define CHECK_ERROR(err)                                              \
  if (err != CL_SUCCESS) {                                            \
    fprintf(stderr, "[%s:%d] ERROR: %d\n", __FILE__, __LINE__, err);  \
    exit(EXIT_FAILURE);                                               \
  }

int main() {
   /* Host/device data structures */
   cl_platform_id platform;
   cl_device_id device;
   cl_context context;
   cl_program program;
   cl_int err;

   /* Program/kernel data structures */
   FILE *program_handle;   
   char *program_buffer, *program_log;
   size_t program_size, log_size;
   cl_kernel *kernels;
   char kernel_name[20];
   cl_uint i, num_kernels;

   /* Access the first installed platform */
   err = clGetPlatformIDs(1, &platform, NULL);
   CHECK_ERROR(err);

   /* Access the first available device */
   err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
   if(err == CL_DEVICE_NOT_FOUND) {
      err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);
   }
   CHECK_ERROR(err);

   /* Create a context */
   context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
   CHECK_ERROR(err);

   /* Read program file and place content into buffer */
   program_handle = fopen(PROGRAM_FILE, "r");
   if(program_handle == NULL) {
      perror("Couldn't find the program file");
      exit(1);
   }
   fseek(program_handle, 0, SEEK_END);
   program_size = ftell(program_handle);
   rewind(program_handle);
   program_buffer = (char*)malloc(program_size+1);
   program_buffer[program_size] = '\0';
   fread(program_buffer, sizeof(char), program_size, program_handle);
   fclose(program_handle);

   /* Create program from file */
   program = clCreateProgramWithSource(context, 1, 
      (const char**)&program_buffer, &program_size, &err);
   CHECK_ERROR(err);
   free(program_buffer);


   /* Build program */
   err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
   if(err != CL_SUCCESS) {

      /* Find size of log and print to std output */
      clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 
            0, NULL, &log_size);
      program_log = (char*) malloc(log_size+1);
      program_log[log_size] = '\0';
      clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 
            log_size+1, program_log, NULL);
      printf("%s\n", program_log);
      free(program_log);
      exit(1);
   }

   /* Find out how many kernels are in the source file */
   err = clCreateKernelsInProgram(program, 0, NULL, &num_kernels);	
   CHECK_ERROR(err);

   /* Create a kernel for each function */
   kernels = (cl_kernel*) malloc(num_kernels * sizeof(cl_kernel));
   clCreateKernelsInProgram(program, num_kernels, kernels, NULL);	

   /* Search for the named kernel */
   printf("Kernels: ");
   for(i=0; i<num_kernels; i++) {					
      clGetKernelInfo(kernels[i], CL_KERNEL_FUNCTION_NAME, 		
            sizeof(kernel_name), kernel_name, NULL);				
      printf("%s ", kernel_name);
   }									
   printf("\n");

   for(i=0; i<num_kernels; i++)					
      clReleaseKernel(kernels[i]);
   free(kernels);
   clReleaseProgram(program);
   clReleaseContext(context);

   return 0;
}
