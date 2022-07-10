/**
	Copyright (C) 2016 Valdemar Lindberg

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>

#include <getopt.h>
#include <unistd.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <getopt.h>
#include <signal.h>

#include <CL/cl.h>
#include <CL/cl_platform.h>

const char *get_cl_error_str(unsigned int errorcode) {
	static const char *errorString[] = {
		"CL_SUCCESS",
		"CL_DEVICE_NOT_FOUND",
		"CL_DEVICE_NOT_AVAILABLE",
		"CL_COMPILER_NOT_AVAILABLE",
		"CL_MEM_OBJECT_ALLOCATION_FAILURE",
		"CL_OUT_OF_RESOURCES",
		"CL_OUT_OF_HOST_MEMORY",
		"CL_PROFILING_INFO_NOT_AVAILABLE",
		"CL_MEM_COPY_OVERLAP",
		"CL_IMAGE_FORMAT_MISMATCH",
		"CL_IMAGE_FORMAT_NOT_SUPPORTED",
		"CL_BUILD_PROGRAM_FAILURE",
		"CL_MAP_FAILURE",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"CL_INVALID_VALUE",
		"CL_INVALID_DEVICE_TYPE",
		"CL_INVALID_PLATFORM",
		"CL_INVALID_DEVICE",
		"CL_INVALID_CONTEXT",
		"CL_INVALID_QUEUE_PROPERTIES",
		"CL_INVALID_COMMAND_QUEUE",
		"CL_INVALID_HOST_PTR",
		"CL_INVALID_MEM_OBJECT",
		"CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
		"CL_INVALID_IMAGE_SIZE",
		"CL_INVALID_SAMPLER",
		"CL_INVALID_BINARY",
		"CL_INVALID_BUILD_OPTIONS",
		"CL_INVALID_PROGRAM",
		"CL_INVALID_PROGRAM_EXECUTABLE",
		"CL_INVALID_KERNEL_NAME",
		"CL_INVALID_KERNEL_DEFINITION",
		"CL_INVALID_KERNEL",
		"CL_INVALID_ARG_INDEX",
		"CL_INVALID_ARG_VALUE",
		"CL_INVALID_ARG_SIZE",
		"CL_INVALID_KERNEL_ARGS",
		"CL_INVALID_WORK_DIMENSION",
		"CL_INVALID_WORK_GROUP_SIZE",
		"CL_INVALID_WORK_ITEM_SIZE",
		"CL_INVALID_GLOBAL_OFFSET",
		"CL_INVALID_EVENT_WAIT_LIST",
		"CL_INVALID_EVENT",
		"CL_INVALID_OPERATION",
		"CL_INVALID_GL_OBJECT",
		"CL_INVALID_BUFFER_SIZE",
		"CL_INVALID_MIP_LEVEL",
		"CL_INVALID_GLOBAL_WORK_SIZE",
	};

	/*	compute error index code. 	*/
	const int errorCount = sizeof(errorString) / sizeof(errorString[0]);
	const int index = -errorcode;

	/*	return error string.	*/
	return (index >= 0 && index < errorCount) ? errorString[index] : "Unspecified Error";
}

/**/
cl_context createclcontext(int *ndevices, cl_device_id **devices) {
	cl_int ciErrNum;
	cl_context context;
	cl_platform_id *platforms;

	cl_context_properties props[] = {CL_CONTEXT_PLATFORM, NULL, NULL};

	unsigned int nDevices;
	unsigned int nPlatforms;
	unsigned int nselectPlatform = 0;

	/*	get platform id.	*/
	ciErrNum = clGetPlatformIDs(0, NULL, &nPlatforms);
	platforms = malloc(sizeof(*platforms) * nPlatforms);
	ciErrNum = clGetPlatformIDs(nPlatforms, platforms, NULL);

	/*	get device ids for the GPUS.	*/
	ciErrNum = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 0, NULL, &nDevices);
	*devices = malloc(sizeof(cl_device_id) * nDevices);
	ciErrNum = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, nDevices, *devices, NULL);

	/*	create context.	*/
	props[1] = platforms[nselectPlatform];
	context = clCreateContext(props, nDevices, *devices, NULL, NULL, &ciErrNum);
	if (context == NULL) {
		fprintf(stderr, "Failed to create OpenCL context. %d\n  [ %s ]", ciErrNum, get_cl_error_str(ciErrNum));
	}

	if (ndevices) {
		*ndevices = nDevices;
	}

	free(platforms);
	return context;
}

/*	Create OpenCL program.	*/
cl_program createProgram(cl_context context, unsigned int nDevices, cl_device_id *device, const char *cfilename) {
	cl_int ciErrNum;
	cl_program program;
	char *source;
	FILE *f;
	long int flen;
	long int nbytes;

	f = fopen(cfilename, "rb");
	if (!f) {
		fprintf(stderr, "Failed loading %s : %s\n", cfilename, strerror(errno));
		return NULL;
	}

	/*	*/
	fseek(f, 0, SEEK_END);
	flen = ftell(f);
	fseek(f, SEEK_SET, 0);

	/*	*/
	source = (char *)malloc(flen + 1);
	nbytes = fread(source, 1, flen, f);
	source[nbytes] = '\0';

	/*	*/
	fclose(f);

	/*	*/
	program = clCreateProgramWithSource(context, 1, (const char **)&source, NULL, &ciErrNum);
	if (program == NULL || ciErrNum != CL_SUCCESS) {
		fprintf(stderr, "Failed to create program %d %s\n", ciErrNum, get_cl_error_str(ciErrNum));
	}

	/*	*/
	ciErrNum = clBuildProgram(program, nDevices, device, "", NULL, NULL);
	if (ciErrNum != CL_SUCCESS) {
		if (ciErrNum == CL_BUILD_PROGRAM_FAILURE) {
			size_t build_log_size = 900;
			char build_log[900];
			size_t build_log_ret;

			ciErrNum = clGetProgramBuildInfo(program, device[0], CL_PROGRAM_BUILD_LOG, build_log_size, build_log,
											 &build_log_ret);
			fprintf(stderr, build_log);
		}
	}

	free(source);
	return program;
}

cl_command_queue createcommandqueue(cl_context context, cl_device_id device) {
	cl_int error;
	cl_command_queue queue;
	cl_command_queue_properties pro = 0;
	queue = clCreateCommandQueue(context, device, pro, &error);
	if (error != CL_SUCCESS) {
		fprintf(stderr, "Failed to create command queue . %d \n", error);
	}
	return queue;
}

typedef struct kernel_platform_t {
	cl_command_queue queue;
	union {
		struct {
			cl_uint maxXworkitems;
			cl_uint nmaxXworkitems;
			cl_uint maxYworkitems;
			cl_uint nmaxYworkitems;
			cl_uint maxZworkitems;
			cl_uint nmaxZworkitems;
		};
		cl_uint globalworksize[6];
	};
} KernelPlatform;

int main(int argc, char **argv) {
	unsigned int status = EXIT_SUCCESS;
	unsigned int human = 0;
	unsigned int x;
	int c;
	int index;
	unsigned int base = 10;
	const char *shortopt = "voHhb:";
	cl_int err;
	size_t ret;
	cl_context context = NULL;
	int nDevices = 0;
	cl_device_id *devices = NULL;
	KernelPlatform *kerplat = NULL;
	cl_mem buf = NULL;
	cl_program program = NULL;
	cl_kernel kernel = NULL;
	cl_uint *PI = NULL;
	size_t workgroup[3] = {0, 1, 1};
	size_t localworksize[3] = {1, 1, 1};
	unsigned int workoffset = 0;
	unsigned int maxglobalworksize = 0;
	unsigned int plen;
	// unsigned int pcarry;
	unsigned int p;
	unsigned long int tmpp = 1;
	const char *clfilepath = WPRIME_SOURCEPATH; /*	TODO check if its possible to simplify this.	*/

	static const struct option longoption[] = {
		{"version", no_argument, 0, 'v'},		{"ocatal", no_argument, 0, 'o'},	 {"human", no_argument, 0, 'h'},
		{"devices", required_argument, 0, 'n'}, {"base", required_argument, 0, 'b'}, {NULL, NULL, NULL, NULL},
	};

	/*	*/
	if (argc <= 1) {
		printf("Required at least one argument.\n");
		return EXIT_FAILURE;
	}

	/*	Iterate through each argument.	 */
	while ((c = getopt_long(argc, (char *const *)argv, shortopt, longoption, &index)) != EOF) {
		switch (c) {
		case 'v':
			printf("version %d.%d.%d.\n", 1, 0, 0);
			return EXIT_SUCCESS;
		case 'o':
			base = 8;
			break;
		case 'h':
			human = 1;
			break;
		case 'b':
			base = atoi(optarg);
			break;
		case 'd':
			base = 10;
			break;
		case 'n':
			break;
		default:
			break;
		}
	}
	p = strtoll(argv[optind], NULL, base);

	/*	Create OpenCL context.	*/
	context = createclcontext(&nDevices, &devices);
	if (context == NULL) {
		status = EXIT_FAILURE;
		goto error;
	}

	/*	*/
	kerplat = (KernelPlatform *)malloc(sizeof(KernelPlatform) * nDevices);
	memset(kerplat, 0, sizeof(KernelPlatform) * nDevices);
	for (x = 0; x < nDevices; x++) {
		clGetDeviceInfo(devices[x], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(kerplat[x].globalworksize),
						&kerplat[x].globalworksize, &ret);
		maxglobalworksize += kerplat[x].maxXworkitems;
	}
	plen = p / maxglobalworksize;

	/*	Create */
	buf = clCreateBuffer(context, CL_MEM_READ_WRITE, maxglobalworksize * sizeof(cl_uint), NULL, &err);
	assert(buf);
	for (x = 0; x < nDevices; x++) {
		kerplat[x].queue = createcommandqueue(context, devices[x]);
		assert(kerplat[x].queue);
		workoffset += kerplat[x].maxXworkitems;
	}
	workoffset = 0;
	program = createProgram(context, nDevices, devices, WPRIME_SOURCEPATH);
	assert(program);
	kernel = clCreateKernel(program, (const char *)"wprime", &err);
	assert(kernel);

	/*	Set kernel function argument values.	*/
	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buf); /**/
	err = clSetKernelArg(kernel, 1, sizeof(cl_uint), &plen);
	err = clSetKernelArg(kernel, 3, sizeof(cl_uint), &p);

	/*	Execute command.	*/
	for (x = 0; x < nDevices; x++) {
		workgroup[0] = kerplat[x].maxXworkitems;
		err = clSetKernelArg(kernel, 2, sizeof(cl_uint), &workoffset);
		err =
			clEnqueueNDRangeKernel(kerplat[x].queue, kernel, 1, NULL /*&localworksize[0]*/, &workgroup[0], 0, 0, 0, 0);
		if (err != CL_SUCCESS) {
			fprintf(stderr, "Failed to clEnqueueNDRangeKernel %d %s\n", err, get_cl_error_str(err));
		}
		workoffset += kerplat[x].maxXworkitems;
	}

	/*	*/
	workoffset = 0;
	PI = malloc(maxglobalworksize * sizeof(cl_uint));

	for (x = (plen * maxglobalworksize) + 1; x < p; x++) {
		tmpp = (tmpp * (unsigned long int)x) % p;
	}

	for (x = 0; x < nDevices; x++) {
		err = clFlush(kerplat[x].queue);
	}
	/*	Fetch result.	*/
	for (x = 0; x < nDevices; x++) {
		err = clEnqueueReadBuffer(kerplat[x].queue, buf, CL_TRUE, workoffset * sizeof(cl_uint),
								  kerplat[x].maxXworkitems * sizeof(cl_uint), &PI[workoffset], 0, NULL, NULL);
		workoffset += kerplat[x].maxXworkitems;
		if (err != CL_SUCCESS) {
			fprintf(stderr, "Failed to clEnqueueReadBuffer %d %s\n", err, get_cl_error_str(err));
		}
	}

	for (x = 0; x < maxglobalworksize; x++) {
		tmpp = (tmpp * PI[x]) % p;
	}
	tmpp = (tmpp + 1) % p;

	printf("%d\n", (unsigned int)tmpp);
	if (!human) {
		printf("%d\n", tmpp == 0 ? 1 : 0);
	} else {
		printf("%lld is prime : %s\n", p, tmpp == 0 ? "true" : "false");
	}

error: /*	Failure point.	*/

	err = clReleaseKernel(kernel);
	err = clReleaseProgram(program);
	for (x = 0; x < nDevices; x++) {
		clFlush(kerplat[x].queue);
		ret = clFinish(kerplat[x].queue);
		err = clReleaseCommandQueue(kerplat[x].queue);
		err = clReleaseDevice(devices[x]);
	}
	err = clReleaseMemObject(buf);
	err = clReleaseContext(context);
	free(devices);
	free(PI);
	free(kerplat);

	return status;
}
