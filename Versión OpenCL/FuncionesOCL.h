#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

//Definición de color para impresión
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

//===========================================================================================================================
//========================================  CODIGOS DE CLASE OPENCL  ========================================================
//===========================================================================================================================


void CodigoError(cl_int err) {
	switch (err) {
		case CL_INVALID_OPERATION : 		printf("\n"); 								break;
		case CL_SUCCESS: 					printf("Ejecución correcta\n"); 			break;
		case CL_BUILD_PROGRAM_FAILURE: 		printf("CL_BUILD_PROGRAM_FAILURE\n"); 		break;
		case CL_COMPILER_NOT_AVAILABLE: 	printf("CL_COMPILER_NOT_AVAILABLE\n"); 		break;
		case CL_DEVICE_NOT_AVAILABLE: 		printf("CL_DEVICE_NOT_AVAILABLE \n"); 		break;
		case CL_DEVICE_NOT_FOUND: 			printf("CL_DEVICE_NOT_FOUND\n"); 			break;
		case CL_INVALID_ARG_INDEX : 		printf("CL_INVALID_ARG_INDEX \n"); 			break;
		case CL_INVALID_ARG_SIZE : 			printf("CL_INVALID_ARG_SIZE \n"); 			break;
		case CL_INVALID_ARG_VALUE: 			printf("CL_INVALID_ARG_VALUE\n");		 	break;
		case CL_INVALID_BUFFER_SIZE : 		printf("CL_INVALID_BUFFER_SIZE \n"); 		break;
		case CL_INVALID_BUILD_OPTIONS: 		printf("CL_INVALID_BUILD_OPTIONS\n"); 		break;
		case CL_INVALID_COMMAND_QUEUE : 	printf("CL_INVALID_COMMAND_QUEUE \n"); 		break;
		case CL_INVALID_CONTEXT: 			printf("CL_INVALID_CONTEXT\n"); 			break;
		case CL_INVALID_DEVICE_TYPE: 		printf("CL_INVALID_DEVICE_TYPE\n"); 		break;
		case CL_INVALID_EVENT: 				printf("CL_INVALID_EVENT\n"); 				break;
		case CL_INVALID_EVENT_WAIT_LIST : 	printf("CL_INVALID_EVENT_WAIT_LIST \n"); 	break;
		case CL_INVALID_GLOBAL_WORK_SIZE : 	printf("CL_INVALID_GLOBAL_WORK_SIZE \n"); 	break;
		case CL_INVALID_HOST_PTR : 			printf("CL_INVALID_HOST_PTR \n"); 			break;
		case CL_INVALID_KERNEL: 			printf("CL_INVALID_KERNEL \n"); 			break;
		case CL_INVALID_KERNEL_ARGS : 		printf("CL_INVALID_KERNEL_ARGS \n"); 		break;
		case CL_INVALID_KERNEL_NAME: 		printf("CL_INVALID_KERNEL_NAME\n"); 		break;
		case CL_INVALID_MEM_OBJECT : 		printf("CL_INVALID_MEM_OBJECT \n"); 		break;
		case CL_INVALID_PLATFORM: 			printf("CL_INVALID_PLATFORM\n"); 			break;
		case CL_INVALID_PROGRAM : 			printf("CL_INVALID_PROGRAM \n"); 			break;
		case CL_INVALID_PROGRAM_EXECUTABLE :printf("CL_INVALID_PROGRAM_EXECUTABLE\n");  break;
		case CL_INVALID_QUEUE_PROPERTIES: 	printf("CL_INVALID_QUEUE_PROPERTIES\n"); 	break;
		case CL_INVALID_VALUE: 				printf("CL_INVALID_VALUE\n"); 				break;
		case CL_INVALID_WORK_DIMENSION : 	printf("CL_INVALID_WORK_DIMENSION \n"); 	break;
		case CL_INVALID_WORK_GROUP_SIZE : 	printf("CL_INVALID_WORK_GROUP_SIZE \n"); 	break;
		case CL_INVALID_WORK_ITEM_SIZE : 	printf("CL_INVALID_WORK_ITEM_SIZE \n"); 	break;
		case CL_OUT_OF_HOST_MEMORY: 		printf("CL_OUT_OF_HOST_MEMORY\n"); 			break;
		case CL_OUT_OF_RESOURCES: 			printf("CL_OUT_OF_RESOURCES\n"); 			break;
		case CL_MEM_OBJECT_ALLOCATION_FAILURE : printf("CL_MEM_OBJECT_ALLOCATION_FAILURE \n"); break;
		case CL_PROFILING_INFO_NOT_AVAILABLE : printf("CL_PROFILING_INFO_NOT_AVAILABLE \n"); break;
		default: printf("Código de error no contemplado\n"); break;
	}
}

/**
 * Interactúa con los dispositivos OpenCL. clGetPlatformIDs obtiene las plataformas disponibles, se invoca 2 veces,
 * una para obtener el número de plataformas y otra para obtener las mencionadas. clGetPlatformInfo obtiene la
 * información para el usuario de una plataforma OpenCL.
 */
cl_int ObtenerPlataformas(cl_platform_id *&platforms,cl_uint &num_platforms){
	cl_int error;
	error = clGetPlatformIDs(0, NULL,&num_platforms);
	if(error!=CL_SUCCESS){
		CodigoError(error);
		return error;
	}
	platforms=(cl_platform_id *)malloc(sizeof(cl_platform_id)*num_platforms);
	error = clGetPlatformIDs(num_platforms,platforms, &num_platforms);
	if(error!=CL_SUCCESS){
		CodigoError(error);
		return error;
	}
	char mensaje[100];
	for(int i=0;i<num_platforms;i++){
		error = clGetPlatformInfo(platforms[i],CL_PLATFORM_NAME,100,mensaje,NULL);
		if(error!=CL_SUCCESS){
			CodigoError(error);
		return error;
		}
		printf("PLATAFORMA %d: %s\n",i,mensaje);
	}
	return CL_SUCCESS;
 }

/**
 * Cuando se han obtenido las plataformas, se obtienen los dispositivos de cada una de ellas. clGetDeviceIDs se invoca 2 veces
 * una para saber cuantos dispositivos hay y otra para obtenerlos. clGetDeviceInfo sirve para obtener información para el usuario
 * de un dispositivo OpenCl concreto.
 */ 
 cl_int ObtenerDispositivos(cl_platform_id platform, cl_device_type device_type, cl_device_id *&devices, cl_uint &num_devices){
	cl_int error;
	error = clGetDeviceIDs(platform,device_type,0,NULL,&num_devices);
	if(error!=CL_SUCCESS){
		CodigoError(error);
		return error;
	}
	devices=(cl_device_id *)malloc(sizeof(cl_device_id)*num_devices);
	error = clGetDeviceIDs(platform,device_type,num_devices,devices,&num_devices);
	if(error!=CL_SUCCESS){
		CodigoError(error);
		return error;
	}
	char mensaje[100];
	for(int i=0;i<num_devices;i++){
		error=clGetDeviceInfo(devices[i],CL_DEVICE_NAME,100,mensaje,NULL);
		if(error!=CL_SUCCESS){
			CodigoError(error);
			return error;
		}
		printf("Dispositivo %d: %s\n",i,mensaje);
	}
	return CL_SUCCESS;
}

//==========================================================================================================================
//========================================    CODIGOS DE VIDEOS/TEORÍA     =================================================
//==========================================================================================================================

//------------------------------------------Video Tema 2 - Contextos y Colas------------------------------------------------
/**
 * Una vez obtenidos los dispositivos, se asocian a contextos que sirven para manejar las cola, los programas, kernel y
 * compartir objetos de memoria de los dispositivos asociados a un contexto.
 */ 
cl_int CrearContexto(cl_platform_id platform, cl_device_id *devices, cl_uint num_devices, cl_context &contexto){
	cl_int error;
	cl_context_properties prop[3];
	prop[0] = CL_CONTEXT_PLATFORM;prop[1] = (cl_context_properties)platform;prop[2] = 0;

	contexto = clCreateContext(prop, num_devices, devices, NULL, NULL, &error);
	if (error != CL_SUCCESS){
		printf(ANSI_COLOR_RED "[ERROR!]  en la creacion del contexto \n" ANSI_COLOR_RED);
		CodigoError(error);
		return error;
	}
	return CL_SUCCESS;
}

/**
 *  Cuando se ha eestablecido el contexxto, se crean las colas para enviar comandos a los dispositivos asociados al contexto
 *  Estos comandos se encolan en la cola de comandos en un orden determinado.
 */ 
cl_int CrearCola(cl_context contexto, cl_device_id device_id, cl_command_queue_properties prop, cl_command_queue &cola){
	cl_int error;
	cola = clCreateCommandQueue(contexto, device_id, prop, &error);
	if (error != CL_SUCCESS){
		printf(ANSI_COLOR_RED "[ERROR!]  en la creacion de la cola \n" ANSI_COLOR_RESET);
		CodigoError(error);
		return error;
	}
	return CL_SUCCESS;
}

//------------------------------------------Video Tema 2 - Programas y Kernels------------------------------------------------

//Dado una ruta de un fichero, leerá todo su contenido y guardar en src
char leerFuentes(char *&src, const char *fileName){
    
	FILE *file = fopen(fileName, "rb");
    if (!file) {
        printf(ANSI_COLOR_RED "[ERROR!]   al abrir el fichero '%s'\n", fileName);printf(ANSI_COLOR_RESET);
        return 0;
    }

	if (fseek(file, 0, SEEK_END))  {
		printf(ANSI_COLOR_RED "[ERROR!]   de posicionamiento en el '%s'\n", fileName);printf(ANSI_COLOR_RESET);
		fclose(file);
		return 0;
    }

    long size = ftell(file);
    if (size == 0)  {
        printf(ANSI_COLOR_RED "[ERROR!]   al calcular el tamaño del fichero '%s'\n", fileName);printf(ANSI_COLOR_RESET);
        fclose(file);
        return 0;
    }

	rewind(file);
    src = (char *)malloc(sizeof(char) * size + 1);
    size_t res = fread(src, 1, sizeof(char) * size, file);
    if (res != sizeof(char) * size)   {
        printf(ANSI_COLOR_RED "[ERROR!]   de lectura del fichero '%s'\n", fileName);printf(ANSI_COLOR_RESET);
        fclose(file);
        free(src);
        return 0;
	}
    src[size] = '\0';
    fclose(file);
	return 1;
}

/**
 * Creación del programa y manejo de errores
 * Se asocia a un programa con extensión .cl, en este proyecto se llama programa.cl donde hay varias funciones escritas en 
 * lenguaje OpenCL, concretamente, potencias y funcion. Lo compila el host indicando el contexto que ejecutará.
 */ 

cl_int CrearPrograma(cl_program &program, cl_context context, cl_uint num_devices, const cl_device_id *device_list, const char *options, const char *fichero) {
	
	cl_int error;
	char *fuente;
	
	if(!leerFuentes(fuente, fichero))	return -1;

	program=clCreateProgramWithSource(context, 1, (const char **)&fuente, NULL, &error);

	if (error != CL_SUCCESS){
		printf(ANSI_COLOR_RED "[ERROR!]   creando el programa\n" ANSI_COLOR_RESET);
		CodigoError(error);
		return error;
	}

	error=clBuildProgram(program, num_devices, device_list, options, NULL, NULL);

	if (error != CL_SUCCESS){
		printf(ANSI_COLOR_RED "[ERROR!]   al compilar el programa\n" ANSI_COLOR_RESET);
		CodigoError(error);
		if(error == CL_BUILD_PROGRAM_FAILURE){
			char buffer[12000];
			clGetProgramBuildInfo(program, device_list[0], CL_PROGRAM_BUILD_LOG, 12000, buffer, NULL);
			printf(ANSI_COLOR_RED "[ERROR!]   de compilacion %s \n", buffer); printf(ANSI_COLOR_RESET);
		}
		return error;
	}
	return CL_SUCCESS;
}

//Creación del KERNEL
cl_int CrearKernel (cl_kernel &kernel, cl_program program, const char *kernel_name) {
	
	cl_int error;
	kernel = clCreateKernel(program, kernel_name, &error);
	if (error != CL_SUCCESS){
		printf( ANSI_COLOR_RED "[ERROR!]   creando el Kernel\n" ANSI_COLOR_RESET);
		CodigoError(error);
		return error;
	}
	return CL_SUCCESS;
}

//-----------------------------------Video Tema 2 - Buffers y Asignación de parámetros------------------------------------------

/**
 * Los BUFFERS son las estructuras de entrada y salida que usan los kernels, reservan memoria en el kernel pudiendo reutilizarla.
 * Esta acción se lleva a cabo mediante cl_mem clCreateBuffer.
 */ 
cl_int CrearBuffer(cl_context context, cl_mem_flags flags, size_t size, void *host_ptr, cl_mem &buffer){

	cl_int error;
	buffer = clCreateBuffer(context, flags, size, host_ptr, &error);

	if(error!=CL_SUCCESS){
		printf(ANSI_COLOR_RED "[ERROR!]   creando el Buffer" ANSI_COLOR_RESET);
		CodigoError(error);
		return error;
	}
	return CL_SUCCESS;
}

/**
 * Asignación de parámetros
 * Cuando ya tenemos los objetos de memoria, se enlazan con los diferentes parámetros del kernel usando la función cl_int clSetKernelArg.
 * Se realiza invocando para cada parámetro del kernel usando – cl_int clSetKernelArg.
 */ 
cl_int AsignarParametro(cl_kernel kernel, cl_uint arg_index, size_t arg_size, const void *arg_value){
	cl_int error;
	error=clSetKernelArg(kernel, arg_index, arg_size, arg_value);

	if(error!=CL_SUCCESS){
		printf(ANSI_COLOR_RED "[ERROR!] al asignar el parámetro" ANSI_COLOR_RESET);
		CodigoError(error);
		return error;
	}
	return CL_SUCCESS;
}
//Enviar Buffer
cl_int EnviarBuffer(cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_write, size_t offset, size_t cb, const void *ptr, cl_uint num_events, const cl_event *event_wait_list, cl_event &event){
	cl_int error;
	error=clEnqueueWriteBuffer(command_queue, buffer, blocking_write, offset, cb, ptr, num_events, event_wait_list, &event);

	if(error!=CL_SUCCESS){
		printf(ANSI_COLOR_RED "[ERROR!] al crear el buffer" ANSI_COLOR_RESET);
		CodigoError(error);
		return error;
	}
	return CL_SUCCESS;
}
//Recibir Buffer
cl_int RecibirBuffer(cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_read, size_t offset, size_t cb, void *ptr, cl_uint num_events, const cl_event *event_wait_list, cl_event &event){
	cl_int error;
	error= clEnqueueReadBuffer(command_queue, buffer, blocking_read, offset, cb, ptr, num_events, event_wait_list, &event);

	if(error!=CL_SUCCESS){
		printf( ANSI_COLOR_RED "[ERROR!] al crear el buffer" ANSI_COLOR_RESET);
		CodigoError(error);
		return error;
	}
	return CL_SUCCESS;
}

/**
 * Ejecución del kernel.
 * El host y el dispositivo pueden counicarse, en este momento se ejecuta el kernel en un dispositivo concreto.
 */ 
cl_int EjecutarKernel (cl_command_queue command_queue,cl_kernel kernel, cl_uint work_dim,const size_t *global_work_offset,const size_t *global_work_size,size_t *local_work_size, cl_uint num_events,const cl_event *event_wait_list, cl_event &event) {
	cl_int error;
	error=clEnqueueNDRangeKernel(command_queue, kernel, work_dim, global_work_offset, global_work_size, local_work_size, num_events, event_wait_list, &event);
	if (error != CL_SUCCESS) {
		printf(ANSI_COLOR_RED "[ERROR!] al ejecutar el kernel " ANSI_COLOR_RESET);
		CodigoError(error);
		return error;
	}
	return CL_SUCCESS;
}
