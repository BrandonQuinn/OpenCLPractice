#include <stdlib.h>
#include <stdio.h>
#include <CL\cl.h>

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
	size_t dataSize;
	cl_uint error;
    char data[4096];

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

int main() 
{
    cl_platform_id* platforms;
    cl_platform_id curPlatform;
    cl_uint numPlatforms;
    cl_device_id curDevice;
    cl_context context;

    // platform
    platforms = GetPlatforms(&numPlatforms);
    PrintPlatformInformation(platforms, numPlatforms);
    curPlatform = platforms[0];
    
    // device
    curDevice = GetFirstGPU(curPlatform);
	PrintDeviceInfo(curDevice);

    // context
    context = CreateContext(curDevice);
    

	getchar();
    free(platforms);
	return 0;
}
