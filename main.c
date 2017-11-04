#include <stdlib.h>
#include <stdio.h>
#include <CL\cl.h>

#define PROGRAM "testkernel.cl"
#define ADD_KERNEL "add"
#define SUB_KERNEL "sub"
#define MULT_KERNEL "mult"
#define DIV_KERNEL "div"

// Return all the platform ids on this machine.
cl_platform_id* GetPlatforms(
		cl_uint* numPlatforms) 
{
	cl_platform_id* platforms;
	cl_uint error;

	error = clGetPlatformIDs(5, NULL, numPlatforms);
	platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * (*numPlatforms));
	error |= clGetPlatformIDs((*numPlatforms), platforms, NULL);
	
	if (error != CL_SUCCESS) {
		printf("Could not get platforms\n");
		exit(EXIT_FAILURE);
	}
	
	return platforms;
}

// Print the vendor and name of all platforms found.
void PrintPlatformInformation(
		cl_platform_id* platforms, 
		cl_uint numPlatforms)
{
	char* platformData;
	size_t dataSize;
	cl_uint error;

	// print out platform name and vendor name
	for (cl_uint i = 0; i < numPlatforms; i++) {
		error = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 0, NULL, &dataSize);
		platformData = (char*) malloc(dataSize);
		error |= clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, dataSize, platformData, NULL);

		printf("Platform found: %s\n", platformData);
		
		free(platformData);
		platformData = NULL;
		dataSize = 0;

		error |= clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, 0, NULL, &dataSize);
		platformData = (char*) malloc(dataSize);
		error |= clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, dataSize, platformData, NULL);

		printf("Vendor: %s\n\n", platformData);
		free(platformData);
	}

	if (error != CL_SUCCESS) {
		printf("Could not print platform information\n");
		exit(EXIT_FAILURE);
	}
}

// Get all GPU devies on the given platform and return the ID
// to the first GPU found.
cl_device_id GetFirstGPU(cl_platform_id platform)
{
	size_t numDevices;
	cl_uint error;
	cl_device_id* devices;
	cl_device_id selectDevice;

	error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, NULL, NULL, &numDevices);
	devices = (cl_device_id*)malloc(sizeof(cl_device_id) * numDevices);
	error |= clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);

	if (error != CL_SUCCESS) {
		printf("Could no find GPU device\n");
		exit(EXIT_FAILURE);
	}

	selectDevice = devices[0];
	free(devices);
	return selectDevice;
}

// Print out the name and vendor of the ginve device.
void PrintDeviceInfo(cl_device_id device)
{
	cl_uint error;
	char data[4096]; size_t dataSize;

	error = clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(data), data, &dataSize);
	printf("\nSelected Device: %s\n", data);
	error = clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(data), data, &dataSize);
	printf("Selected Device Vendor: %s\n", data);
}

// Create a context for the given device.
cl_context CreateContext(cl_device_id device)
{
	cl_uint error;
	cl_context context;

	context = clCreateContext(NULL, 1, &device, NULL, NULL, &error);

	if (error != CL_SUCCESS) {
		printf("Could not create context\n");
		exit(EXIT_FAILURE);
	}

	return context;
}

// Read in the program source and create a cl_program from it.
cl_program CreateProgram(cl_context context)
{
	cl_program program;
	cl_uint error;
	FILE* handle; size_t programSize;
	char* programBuf;

	// read the program source
	handle = fopen(PROGRAM, "r");
	if (handle == NULL) {
		printf("Could not open program file\n");
		exit(EXIT_FAILURE);
	}

	fseek(handle, 0, SEEK_END);
	programSize = ftell(handle);
	rewind(handle);
	programBuf = (char*) malloc(sizeof(char) * programSize);
	programBuf[programSize] = '\0';
	fread(programBuf, sizeof(char), programSize, handle);
	fclose(handle);

	program = clCreateProgramWithSource(context, 1, (const char**) & programBuf, &programSize, &error);
	
	if (error != CL_SUCCESS) {
		printf("Could not create program with source\n");
		exit(EXIT_FAILURE);
	}

	free(programBuf);
	return program;
}

// Build the program and if it fails print out the log information.
void BuildProgram(cl_program program, cl_device_id device)
{
	cl_uint error, logSize;
	char* programLog;

	error = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

	if (error < 0) {
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);
		programLog = (char*)malloc(logSize + 1);
		programLog[logSize] = '\0';
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize + 1, programLog, NULL);
		printf("%s\n", programLog);
		free(programLog);
		exit(EXIT_FAILURE);
	}
}

int main() 
{
	cl_platform_id* platforms;
	cl_platform_id curPlatform;
	cl_uint numPlatforms;
	cl_device_id curDevice;
	cl_context context;
	cl_program program;

	// platform
	platforms = GetPlatforms(&numPlatforms);
	PrintPlatformInformation(platforms, numPlatforms);
	curPlatform = platforms[0];
	
	// device
	curDevice = GetFirstGPU(curPlatform);
	PrintDeviceInfo(curDevice);

	context = CreateContext(curDevice);	
	program = CreateProgram(context);
	BuildProgram(program, curDevice);



	getchar();

	clReleaseProgram(program);
	clReleaseContext(context);
	free(platforms);
	return 0;
}
