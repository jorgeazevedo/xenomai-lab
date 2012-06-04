/*
 * Xenomai Lab
 * Copyright (C) 2011 Jorge Azevedo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BLOCKIO_H
#define BLOCKIO_H


#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>
#include <native/mutex.h>
#include <native/queue.h>
#include <native/pipe.h>
#include <native/heap.h>

#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
/**
 * Because this library is also compiled from Qt,
 * we can't define gnu_source, use_gnu. To avoid
 * a compilation warning, we declare feenableexcept
 * ourselves. This is a cheap hack.
 */
//#define _GNU_SOURCE
//#define __USE_GNU
#include <fenv.h>
int feenableexcept(int excepts);

#include <unistd.h>
#include <sys/mman.h>   //MLOCKALL
#include <stdlib.h>     //exit(1))
#include <sys/types.h> //open()
#include <fcntl.h>	//open() related
#include <argp.h>

#include "mtrx.h"
#include "settings.h"

//Useful Macros
//Commenting this define disables debug messages
#define DEBUGON

#define ERROR(...) fprintf(stderr, "E: %17s: ", __PRETTY_FUNCTION__),fprintf(stderr,__VA_ARGS__),fflush(stderr),exit(1)
#define RETERROR(...) {fprintf(stderr, "E: %17s: ", __PRETTY_FUNCTION__);fprintf(stderr,__VA_ARGS__);fflush(stderr);return 1;}

#ifdef DEBUGON

        #define DEBUG(...) printf("%20s: \t", __PRETTY_FUNCTION__),printf(__VA_ARGS__)
#else
        #define	DEBUG(...)
#endif

#define CHAR_BUFFER_SIZE 4096
#define MAX_MESSAGE_LENGTH 1
//the default is around ~63 if Matrix is 8x8)
#define STACK_SIZE_IN_MATRIX_MUL 120

#ifdef __cplusplus
extern "C"
{
#endif

struct ioelements
{
        //For actual access
        RT_PIPE *output_pipes;
        RT_QUEUE *input_queues,*output_queues;

        //Names, count and init values
        char** input_strings,**output_strings,
                **outputp_strings;
        short input_num,output_num,outputp_num;
	Matrix* input_init,*output_init;

        //These hold the inputs
        Matrix input_result[10];
        Matrix inputp_result[10];

        //configuration file
        char *config_file;
        //block name
        char *block_name;
};


extern struct ioelements io;

extern int settings_owner;
extern RT_HEAP settings_heap;
extern Settings *settings;
extern struct global_settings* gs;
/*
 * gs_mtx ensures mutual exclusion
 * on the gs structure from concurrent
 * access from settings GUI. This isn't in ioelements
 * because Xenomai doesn't like it. Don't ask me.
 */
extern RT_MUTEX gs_mtx;

/*
 * main_task is the RT_TASK for main()
 * we need main() to be an RT_TASK because
 * load/save_settings uses RT_MUTEX, which 
 * can only be called from an RT context.
 * This is global for the same reason given
 * for gs_mtx. Because yes.
 */

extern RT_TASK loop_task;
extern RT_TASK main_task;

/* Every loop is while(running). On ctrl-c
 * and other signals, running turns false
 * and the loops exit
 */
extern int running;

void initialize_block(int argc, char* argv[],size_t struct_size, int min_inputs,int min_outputs);
void finalize_block();

void wait_for_task_end();
void start_task(int priority, void* task_function);
void func_try(int ret, char*func);

void* safe_malloc(int bytes);

void parse_args(int argc, char* argv[]);
void free_args(void);

void create_io(void);
void assert_io_min(int min_input, int min_output);
void free_io(void);

void read_inputs();
void read_input_queues();
void write_outputs(Matrix sample);
void write_output_queues(Matrix* sample);

int register_mutex(RT_MUTEX* mut, char* name);
void free_mutex(RT_MUTEX* mut);

// void* create_shm(RT_HEAP* heap, char* heap_name, size_t size);
void* create_shm(RT_HEAP* heap, char* heap_name, size_t size, int* create);
void delete_shm(RT_HEAP* heap,void *pointer);
void* bind_shm(RT_HEAP* heap, char* heap_name,size_t size);
//void unbind_shm(RT_HEAP* heap, void* pointer);
void unbind_shm(RT_HEAP* heap);


//these are all relative to settings
void get_matrix(char* section, char* key, Matrix* M1);
void store_matrix(char* section, char* key, Matrix* M1);
void get_string(char* section, char* key, char* str);
void store_string(char* section, char* key, char* str);
void get_double(char* section, char* key, double* value);
void store_double(char* section, char* key, double value);
void get_int(char* section, char* key, int* value);
void store_int(char* section, char* key, int value);
int load_settings(char* config_file, size_t size);
int save_settings(char* config_file);
int update_settings(char* config_file);
int am_alone(char* heap_name);
//int save_settings(char* config_file, RT_MUTEX* mtx,int rt_context);
//PROPOSED:
//int settings_load(char* config_file, size_t size, int rt_context);
//int settings_save(char* config_file, int rt_context);
void settings_lock(RT_MUTEX* mtx);	
void settings_unlock(RT_MUTEX* mtx);
void get_task_priority(int *value);

void stop(int signum);
void fp_exception(int sig, siginfo_t *siginfo, void *context);

#ifdef __cplusplus
}
#endif


#endif // BLOCKIO_H
