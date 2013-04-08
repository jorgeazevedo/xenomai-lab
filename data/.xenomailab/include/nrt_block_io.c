/*
 * Xenomai Lab
 * Copyright (C) 2013 Jorge Azevedo
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

#include "nrt_block_io.h"

#define MAX_MESSAGE_LENGTH 1

//void test(void);

int stopflag=1;
struct ioelements io;
RT_HEAP settings_heap;
Settings *settings;

const char *argp_program_version =
"XenomaiLab blocks 1.0";
const char *argp_program_bug_address =
"<jorge.azevedo@ua.pt>";
static char doc[] =
"XenomaiLab Non-Real Time Blocks -- Self-contained Linux tasks used by Xenomailab\vExamples: display -i error,setpoint pid_project/display1";
static char args_doc[] = "CONFIG_FILE";

/* The options we understand. */
static struct argp_option options[] = {
        {"input",   'i', "INPUTS", 0,
        "Comma separated list of input pipes" },
        { 0 }
};

/* Used by main to communicate with parse_opt. */
struct arguments
{
        char *config_file;
        char *input_pipes;
};

/* Parse a single option. */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
/* Get the input argument from argp_parse, which we
  know is a pointer to our arguments structure. */
struct arguments *arguments = (struct arguments *)state->input;

        switch (key)
         {
         case 'i':
           arguments->input_pipes = arg;
           break;

         case ARGP_KEY_ARG:
           if (state->arg_num >= 1)
             /* Too many arguments. */
             argp_usage (state);

           /* arguments->config_file[state->arg_num] = arg; */
           arguments->config_file = arg;

           break;

         case ARGP_KEY_END:
           if (state->arg_num <1 )
             /* Not enough arguments. */
             argp_usage (state);
           break;

         default:
           return ARGP_ERR_UNKNOWN;
         }
        return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };

void* safe_malloc(int bytes){
        void* mem_add;

        if(bytes)
                mem_add=malloc(bytes);
        else
                return NULL;

        if(mem_add==NULL)
                ERROR("Malloc failed, out of memory!\n");
        else
                return mem_add;

}


int parse_string(char* original_string, short* num, char***string_vector, Matrix** mat_vector){
	int i,strlen,value=0;
	char buf[50];

        if(original_string[0] == '\0'){
                *num=0;
        }
        else{
		//fast forward to first letter
		//When we call ./block "-i a", instead of .block -i a
		//the space is not taken out, so we have to take it out ourselves
		for(;*original_string==' ';original_string++);

                //numbers of strings==number of commas+1
                for(i=0,*num=1;original_string[i]!='\0';i++)
                        (*num)+=(original_string[i]==',')?1:0;

                //get space for pointer to those strings
                string_vector[0]=(char**)safe_malloc((*num)*sizeof(char*));

		//reserve space for initializers
		*mat_vector=(Matrix*)safe_malloc((*num)*sizeof(Matrix));
		
                for(i=0;i<*num;i++){

                        //ghetto strlen
                        for(strlen=0;(original_string[strlen]!=',')&&
				     (original_string[strlen]!='\0')&&
				     (original_string[strlen]!='=');strlen++);

			//if we have init value, mark for processing
                        if(original_string[strlen]=='=')
				value=1;
			
                        //terminate string
                        original_string[strlen]='\0';

                        //name
                        string_vector[0][i]=(char*)safe_malloc((strlen+1)*sizeof(char));
			//strcp
                        sprintf(string_vector[0][i],"%s",original_string);

                        //discard copied string
                        original_string+=strlen+1;

			if(value){
				//go to next stop condition
				for(strlen=0;(original_string[strlen]!=',')&&
					     (original_string[strlen]!='\0');strlen++);

				//terminate string
				original_string[strlen]='\0';

                        	sprintf(buf,"%s", original_string);
				//DEBUG("-%d->%s\n",i,buf);
				mat_vector[0][i]=new_matrix(buf);
				
				//discard copied string
				original_string+=strlen+1;

				value=0;
			}
			else{
				mat_vector[0][i]=empty_matrix(1,1);
			}
		}
	}

	return 0;
}

void get_block_name(void){

}

void get_configuration_file(void){

}

void get_input_pipes(void){

}

void parse_args(int argc, char* argv[]){
        struct arguments arguments;
        int i,strlen=0;

        /* Default values. */
        arguments.input_pipes = "";

        io.input_num=0;
        io.output_num=0;
        io.inputp_num=0;
        io.outputp_num=0;

        // Parse our arguments
        argp_parse (&argp, argc, argv, 0, 0, &arguments);

	get_block_name();
	get_configuration_file();
	get_input_pipes();

        //Block name
        //ghetto strlen
        for(strlen=0;argv[0][strlen]!='\0';strlen++);

        //from end to start, search for /
        for(i=strlen;argv[0][i]!='/';i--);

        //this will give us block_name plus a '\0'
        io.block_name=(char*)safe_malloc((strlen-i)*sizeof(char));

        sprintf(io.block_name,"%s", argv[0]+i+1);

        //Configuration file
        for(strlen=0;arguments.config_file[strlen]!='\0';strlen++);

        io.config_file=(char*)safe_malloc((strlen+1)*sizeof(char));

        sprintf(io.config_file,"%s", arguments.config_file);


        //Inputp
        if(arguments.input_pipes[0] == '\0'){
                io.inputp_num=0;
        }
        else{
		for(;*arguments.input_pipes==' ';arguments.input_pipes++);
                //numbers of strings==number of commas+1
                for(i=0,io.inputp_num=1;arguments.input_pipes[i]!='\0';i++)
                        io.inputp_num+=(arguments.input_pipes[i]==',')?1:0;

                DEBUG("%d inputp strings found\n",io.inputp_num);

                //get space for pointer to those strings
                io.inputp_strings=(char**)safe_malloc(io.inputp_num*sizeof(char*));

                for(i=0;i<io.inputp_num;i++){

                        //ghetto strlen
                        for(strlen=0;(arguments.input_pipes[strlen]!=',')&&(arguments.input_pipes[strlen]!='\0');strlen++);

                        //terminate string
                        arguments.input_pipes[strlen]='\0';

                        //name
                        io.inputp_strings[i]=(char*)safe_malloc(36+((strlen+1)*sizeof(char)));

                        sprintf(io.inputp_strings[i],"/proc/xenomai/registry/native/pipes/%s", arguments.input_pipes);

                        //discard copied string
                        arguments.input_pipes+=strlen+1;
                }
        }

	assert_io_min(1,0);

        DEBUG("Block name is %s\n",io.block_name);

        DEBUG("Config file is %s\n",io.config_file);

        for(i=0;i<io.inputp_num;i++)
                DEBUG("%d - %s\n",i,io.inputp_strings[i]);

}


void free_args(void){
        short i;

        free(io.block_name);

        free(io.config_file);

        //free strings first, string vector second
        for(i=0;i<io.inputp_num;i++)
                free(io.inputp_strings[i]);
        free(io.inputp_strings);

}

void assert_io_min(int min_input, int min_output)
{
        if((io.input_num+io.inputp_num)<min_input)
                ERROR("This block needs at least %d input(s), not %d!\n",min_input,io.input_num);
}

void create_io(void){
        int i;

        io.input_pipes=(int*)safe_malloc(io.inputp_num*sizeof(int));
        for(i=0;i<io.inputp_num;i++){
                if((io.input_pipes[i]=open(io.inputp_strings[i],O_RDWR))<0)
                        ERROR("Failed to open pipe at %s!\n",io.inputp_strings[i]);
        }

}

void free_io(void){
        int i;

        for(i=0;i<io.inputp_num;i++)
                close(io.input_pipes[i]);
        free(io.input_pipes);

}


void read_inputs()
{
        short i;
	int ret;
        //double sample;
	//PROPOSED:
	Matrix sample;

	for(i=0;i<io.inputp_num;i++){
		if(read(io.input_pipes[i],&sample,sizeof(sample)) < 0)
			stopflag=0;
		
		io.inputp_result[i]=sample;
		//DEBUG("%s-->%4.2f\t", io.inputp_strings[i]+36,io.inputp_result[i]);
	}
//DEBUG("\n");

	/*
	 * After this read_inputs we want the settings
	 * to be "static"
	 */
	//settings_lock(&gs_mtx);
}

void write_outputs(Matrix sample)
{
	//settings_unlock(&gs_mtx);

}

void register_mutex(RT_MUTEX* mut, char* name){

	//This is some (not so) clever trickery to insure
	//a unique name for the mutex.
	char buf[100];
	sprintf(buf,"%s%s",name,"-gs-mutex");

        DEBUG("register_mutex: trying to create and bind %s\n",buf);

        switch (rt_mutex_create(mut,buf)){
                case 0://sucess!
                        DEBUG("Mutex creation done!\n");
                        break;
                case -EEXIST:
                        DEBUG("EEXIST: Failed to create mutex, \n");
                        if(rt_mutex_bind(mut,buf,TM_INFINITE))
                                ERROR("binding failed\n");
                        else
                                DEBUG("bind succeed\n");

                        break;
                case -EPERM:
                        ERROR("EPERM: Invalid context for calling mutex\n");
		case -ENOMEM:
			ERROR("Creation said -ENOMEM\n");
		case -EINVAL:
			ERROR("Got the errorr, it's -EINVAL!!\n");
		default:
			DEBUG("Error in creation!\n");
			break;
        }
}


void free_mutex(RT_MUTEX* mut){
        rt_mutex_unbind(mut);
        //rt_mutex_delete(mut);

}


void* create_shm(RT_HEAP* heap, char* heap_name, size_t size){
        void *pointer;


        if(rt_heap_create(heap,heap_name,size,H_SHARED))
                ERROR("Failed to create heap %s!\n",heap_name);

        rt_heap_alloc(heap,size,TM_NONBLOCK,&pointer);

        return pointer;

}

void delete_shm(RT_HEAP* heap,void *pointer){

	rt_heap_free(heap,pointer);
	rt_heap_delete(heap);

	return;
}

void* bind_shm(RT_HEAP* heap, char* heap_name,size_t size){
    void* pointer;
    int error=0;
    
    //try to bind to heap
    if(rt_heap_bind(heap,heap_name,TM_NONBLOCK))
        error=1;
    else{
        if(rt_heap_alloc(heap,0,TM_NONBLOCK,&pointer))
            error=1;
    }
    
    //if errors occured, 
    if(error){
        DEBUG("bind_shm: going for malloc\n");
        return safe_malloc(size);
    }
    else{
        DEBUG("bind_shm: no errors, got heap\n");
        return pointer;
    }

}

void unbind_shm(RT_HEAP* heap, void* pointer){
	RT_HEAP_INFO info;

	info.usedmem=0;
	rt_heap_inquire(heap,&info);

	//if no heap exists, usedmem will remain unchanged
	if(info.usedmem)
		rt_heap_unbind(heap);
	else
		free(pointer);

}

void get_matrix(char* section, char* key, Matrix* M1){
        char buf[50];

	get_string(section,key,buf);

	*M1=new_matrix(buf);
}

void store_matrix(char* section, char* key, Matrix* M1){
	char buf[50];

	
	matrix_string(M1,buf);
	store_string(section,key,buf);

	return;
}

void get_string(char* section, char* key, char* str){
        char buf[50];

        if(!settings_get(settings,section,key,buf,sizeof(buf)))
                DEBUG("Did not find %s!\n",key);
        sprintf(str,"%s",buf);

}

void store_string(char* section, char* key, char* str){
        settings_set(settings, section, key, str);
}

void get_double(char* section, char* key, double* value){
        char buf[25];

        if(!settings_get(settings,section,key,buf,sizeof(buf)))
                DEBUG("Did not find %s!\n",key);
        *value=atof(buf);

}

void store_double(char* section, char* key, double value){
        char buf[25];

        sprintf(buf,"%4.2f",value);
        settings_set(settings, section, key, buf);
}

void get_int(char* section, char* key, int* value){
        char buf[25];

        if(!settings_get(settings,section,key,buf,sizeof(buf)))
                DEBUG("Did not find %s!\n",key);
        *value=atoi(buf);

}

void store_int(char* section, char* key, int value){
        char buf[25];

        sprintf(buf,"%d",value);
        settings_set(settings, section, key, buf);
}

/*
 * These functions are implemented by the includer
 * ex: ~ /.xenomailab/blocks/pid/pid_settings.c
 */

void load_gs(char *name);
void unload_gs(void);

void settings_lock(RT_MUTEX* mtx){

	switch(rt_mutex_acquire(mtx, TM_INFINITE)){
                case -EINVAL:
                        DEBUG("settings_lock: error -EINVAL\n");
			break;
		case -EIDRM:
                        DEBUG("settings_lock: error -EIDRM\n");
			break;
		case -EWOULDBLOCK:
                        DEBUG("settings_lock: error -EWOULDBLOCK\n");
			break;
		case -EINTR:
                        DEBUG("settings_lock: error -EINTR\n");
			break;
		case -ETIMEDOUT:
                        DEBUG("settings_lock: error -ETIMEDOUT\n");
			break;
		case -EPERM:
                        DEBUG("settings_lock: error -EPERM\n");
			break;
	}
}
void settings_unlock(RT_MUTEX* mtx){

	switch(rt_mutex_release(mtx)){
		case -EINVAL:
                        DEBUG("settings_unlock: error -EINVAL\n");
			break;
		case -EIDRM:
                        DEBUG("settings_unlock: error -EIDRM\n");
			break;
		case -EPERM:
                        DEBUG("settings_unlock: error -EPERM\n");
			break;
	}

}

/*
 * Prepare global settings structure
 */

int load_settings(char * config_file,size_t size, int rt_context){
        FILE *f;
        char buf[100];

        sprintf(buf,"%s%s%s%s",getenv("HOME"),"/.xenomailab/workspace/",config_file,".conf");
        f=fopen(buf, "r");
        if (f==NULL)
                ERROR("load_settings: failed to open file for read!\n");

        settings = settings_open(f);
        fclose(f);
        if (settings==NULL)
                ERROR("load_settings: failed to open settings!\n");

        /*
         * If we're in real-time context, we will create the shm, otherwise we'll just bind
         * to it. It is expected that the GUI is the one that try to bind.
         */
	if(rt_context)
                gs=(struct global_settings*)create_shm(&settings_heap,config_file,size);
	else
                gs=(struct global_settings*)bind_shm(&settings_heap,config_file,size);


        //register_mutex(&gs_mtx,config_file);
	//settings_lock(&gs_mtx);

        load_gs(config_file);

	//settings_unlock(&gs_mtx);

        DEBUG("load_settings: finished load_settings\n");
	return 0;

}

int update_settings(char* config_file){
	FILE *f;
	char buf[100];

	DEBUG("save_settings: going to open file for write\n");

	sprintf(buf,"%s%s%s%s",getenv("HOME"),"/.xenomailab/workspace/",config_file,".conf");
	f = fopen(buf, "w");
	if (f == NULL) {
	    DEBUG("save_settings: failed to open %s for write!\n",buf);
	}

	//settings_lock(&gs_mtx);
	
	unload_gs();

	//settings_unlock(&gs_mtx);

	if(!settings_save(settings, f))
	    DEBUG("save_settings: did not save settings!\n");

	fclose(f);
	
	return 0;
}


int save_settings(char* config_file, int rt_context){

        update_settings(config_file);

        settings_delete(settings);

        if(rt_context)
                delete_shm(&settings_heap,gs);
        else
                unbind_shm(&settings_heap,gs);


	free_mutex(&gs_mtx);
        return 0;
}


void stop(int signum){

        DEBUG("\nCaught signal %d, exiting\n", signum);
        stopflag=0;
}
