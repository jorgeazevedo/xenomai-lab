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
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>     //exit(1))
#include <sys/types.h> //open()
#include <fcntl.h>	//open() related
#include <argp.h>

#include "mtrx.h"
#include "settings.h"


//Useful Macros
//Commenting this define disables debug messages
#define DEBUGON

#define ERROR(...) fprintf(stderr, "E: %s: ", __PRETTY_FUNCTION__),fprintf(stderr,__VA_ARGS__),exit(1)

#ifdef DEBUGON

        #define DEBUG(...) printf("%s: ", __PRETTY_FUNCTION__),printf(__VA_ARGS__)
#else
        #define	DEBUG(...)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct ioelements
{
        //For actual access
        int *input_pipes;	//this hold file descriptors
        RT_PIPE *output_pipes;
        RT_QUEUE *input_queues,*output_queues;

        //Ensure mutual exclusion on
        //a resource subject to contention.
        //This is normaly the I\O device being used
        //in this case the LPT port.
        RT_MUTEX mutex_lpt;

        //Names, count and init values
        char** input_strings,**output_strings,
                **inputp_strings,**outputp_strings;
        short input_num,output_num,inputp_num,outputp_num;
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

extern RT_HEAP heap;
extern Settings *settings;
extern struct global_settings* gs;
extern RT_MUTEX gs_mtx;

//Every loop is while(stopflag). On ctrl-c
//and other signals, stopflag turns false
//and the loops exit
extern int stopflag;

void* safe_malloc(int bytes);

void parse_args(int argc, char* argv[]);
void free_args(void);

void create_io(void);
void assert_io_min(int min_input, int min_output);
void free_io(void);

void read_inputs();
void write_outputs(Matrix sample);

void register_mutex(RT_MUTEX* mut, char* name);
void free_mutex(RT_MUTEX* mut);

void* create_shm(RT_HEAP* heap, char* heap_name, size_t size);
void delete_shm(RT_HEAP* heap,void *pointer);
void* bind_shm(RT_HEAP* heap, char* heap_name,size_t size);
void unbind_shm(RT_HEAP* heap, void* pointer);


//these are all relative to settings
void get_matrix(char* section, char* key, Matrix* M1);
void store_matrix(char* section, char* key, Matrix* M1);
void get_string(char* section, char* key, char* str);
void store_string(char* section, char* key, char* str);
void get_double(char* section, char* key, double* value);
void store_double(char* section, char* key, double value);
void get_int(char* section, char* key, int* value);
void store_int(char* section, char* key, int value);
int load_settings(char* config_file, size_t size, int rt_context);
int save_settings(char* config_file, int rt_context);
int update_settings(char* config_file);
//int save_settings(char* config_file, RT_MUTEX* mtx,int rt_context);
//PROPOSED:
//int settings_load(char* config_file, size_t size, int rt_context);
//int settings_save(char* config_file, int rt_context);
void settings_lock(RT_MUTEX* mtx);	
void settings_unlock(RT_MUTEX* mtx);

void stop(int signum);

#ifdef __cplusplus
}
#endif


#endif // BLOCKIO_H
