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

#include "rt_block_io.h"

#define QUEUE_SIZE 256
#define MAX_MESSAGE_LENGTH 1
#define CHAR_BUFFER_SIZE 300

//void test(void);

int running=1;
struct ioelements io;

int settings_owner;
RT_HEAP settings_heap;
Settings *settings;
RT_MUTEX gs_mtx;

RT_TASK loop_task;
RT_TASK main_task;

const char *argp_program_version =
"XenomaiLab blocks 1.0";
const char *argp_program_bug_address =
"<jorge.azevedo@ua.pt>";
static char doc[] =
"XenomaiLab Real-Time Blocks -- Self-contained Xenomai tasks used by Xenomailab\vExamples: gain -i sensor -o controler -p observer1,observer2 gain4\n          pid -i et -o ut pid_project/pid1";
static char args_doc[] = "CONFIG_FILE";

/* The options we understand. */
static struct argp_option options[] = {
        {"input",   'i', "INPUTS", 0,
        "Comma separated list of input queues", 0},
        {"output",   'o', "OUTPUTS", 0,
        "Comma separated list of output queues", 0},
        {"output-pipes",   'p', "OUTPUTPIPES", 0,
        "Comma separated list of non real-time output pipes", 0},
	{ 0,0,0,0,0,0 }
};

/* Used by main to communicate with parse_opt. */
struct arguments
{
        char *config_file;
        char *input_queues;
        char *output_queues;
        char *output_pipes;
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
           arguments->input_queues = arg;
           break;
         case 'o':
           arguments->output_queues = arg;
           break;
         case 'p':
           arguments->output_pipes = arg;
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
static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

void initialize_block(int argc, char* argv[],size_t struct_size, int min_inputs,int min_outputs){

        mlockall(MCL_CURRENT|MCL_FUTURE);

	parse_args(argc, argv);
	
	assert_io_min(min_inputs, min_outputs);

	func_try(
        	rt_task_shadow(&main_task,NULL,0,0),
		"rt_task_shadow");

	//On SIGINT (Ctrl-C), SIGTERM (kill pid) call stop() (from rt_block_io.h)
	signal(SIGTERM, stop);
	signal(SIGINT, stop);

	create_io();

        if(load_settings(io.config_file,struct_size))
            ERROR("load_settings has failed");
}

void finalize_block(){

        if(save_settings(io.config_file))
            ERROR("save_settings has failed");

	free_args();
	free_io();
	
	func_try(
		rt_task_delete(&loop_task),
		"rt_task_deleteLoop");

	func_try(
		rt_task_delete(&main_task),
		"rt_task_deleteLoop");

	DEBUG("Terminated\n");
}

void func_try(int ret, char*func){
	DEBUG("%s\n",func);
	switch(ret){
		case 0://sucess!
			DEBUG("Success!\n");
			break;
		case -EEXIST:
			ERROR("EEXIST!\n");
			break;
		case -EPERM:
			ERROR("EPERM: Invalid context for calling mutex\n");
		case -ENOMEM:
			ERROR("-ENOMEM\n");
		case -EINVAL:
			ERROR("-EINVAL\n");
		case -EBUSY:
			ERROR("-EBUSY\n");
		case -EINTR:
			ERROR("-EINTR\n");
		case -EIDRM:
			ERROR("-EIDRM\n");
		case -EWOULDBLOCK:
			ERROR("-EWOULDBLOCK\n");
		default:
			ERROR("Failed with unknonwn error!\n");
			break;
	}
}

void start_task(int priority, void* task_function){

	func_try(
		rt_task_create(&loop_task, NULL, 0, priority, T_JOINABLE),
		"rt_task_create");

	func_try(
		rt_task_start(&loop_task, task_function, NULL),
		"rt_task_start");
	
	DEBUG("%s loop about to start\n",io.block_name);
	//This fflush is important! Without it  before the loop, interface will fail!
	fflush(stdout);
}

void wait_for_task_end(){

	rt_task_join(&loop_task);

}

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
	char buf[CHAR_BUFFER_SIZE];

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
				if(new_matrix_safe(&mat_vector[0][i],buf))
					ERROR("Failed to parse given matrix\n");
				
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

void parse_args(int argc, char* argv[]){
        struct arguments arguments;
        int i,strlen=0;
	char buf[CHAR_BUFFER_SIZE];

        /* Default values. */
        arguments.input_queues = "";
        arguments.output_queues = "";
        arguments.output_pipes = "";

        io.input_num=0;
        io.output_num=0;
        io.outputp_num=0;

        // Parse our arguments
        argp_parse (&argp, argc, argv, 0, 0, &arguments);

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
        
	DEBUG("Block name is %s\n",io.block_name);
        DEBUG("Config file is %s\n",io.config_file);

        //Process
	parse_string(arguments.input_queues,&io.input_num,&io.input_strings,&io.input_init);

	DEBUG("Input queues:\n");
        for(i=0;i<io.input_num;i++){
		matrix_string(io.input_init+i,buf);
                DEBUG("%d - %s with value %s\n",i,io.input_strings[i],buf);
	}
	if(i==0)
		DEBUG("None!\n");

	parse_string(arguments.output_queues,&io.output_num,&io.output_strings,&io.output_init);

	DEBUG("Output queues\n");
        for(i=0;i<io.output_num;i++){
		matrix_string(io.output_init+i,buf);
                DEBUG("%d - %s with value %s\n",i,io.output_strings[i],buf);
	}
	if(i==0)
		DEBUG("None!\n");

        //Outputp
        if(arguments.output_pipes[0] == '\0'){
                io.outputp_num=0;
        }
        else{
		for(;*arguments.output_pipes==' ';arguments.output_pipes++);
                //numbers of strings==number of commas+1
                for(i=0,io.outputp_num=1;arguments.output_pipes[i]!='\0';i++)
                        io.outputp_num+=(arguments.output_pipes[i]==',')?1:0;

                DEBUG("%d outputp strings found\n",io.outputp_num);

                //get space for pointer to those strings
                io.outputp_strings=(char**)safe_malloc(io.outputp_num*sizeof(char*));

                for(i=0;i<io.outputp_num;i++){

                        //ghetto strlen
                        for(strlen=0;(arguments.output_pipes[strlen]!=',')&&(arguments.output_pipes[strlen]!='\0');strlen++);

                        //terminate string
                        arguments.output_pipes[strlen]='\0';

                        //name
                        io.outputp_strings[i]=(char*)safe_malloc((strlen+1)*sizeof(char));

                        sprintf(io.outputp_strings[i],"%s", arguments.output_pipes);

                        //discard copied string
                        arguments.output_pipes+=strlen+1;
                }
        }

	DEBUG("Output pipes\n");
        for(i=0;i<io.outputp_num;i++)
                DEBUG("%d - %s\n",i,io.outputp_strings[i]);
	if(i==0)
		DEBUG("None!\n");
}


void free_args(void){
        short i;

        free(io.block_name);

        free(io.config_file);

        //free strings first, string vector second
        for(i=0;i<io.input_num;i++)
                free(io.input_strings[i]);
        free(io.input_strings);

        for(i=0;i<io.output_num;i++)
                free(io.output_strings[i]);
        free(io.output_strings);

        for(i=0;i<io.outputp_num;i++)
                free(io.outputp_strings[i]);
        free(io.outputp_strings);

}

void assert_io_min(int min_input, int min_output)
{
        if(io.input_num<min_input)
                ERROR("This block needs at least %d input(s), not %d!\n",min_input,io.input_num);
        if((io.output_num+io.outputp_num)<min_output)
                ERROR("This block needs at least %d output(s)!\n",min_output);
}

void create_io(void){
        int i;
        //double init_value=0.0;
	//PROPOSED:
	Matrix init_value;
	init_value=empty_matrix(1,1);

        io.input_queues=(RT_QUEUE*)safe_malloc(io.input_num*sizeof(RT_QUEUE));
        for(i=0;i<io.input_num;i++){

                if(rt_queue_create(io.input_queues+i,io.input_strings[i],
                QUEUE_SIZE,MAX_MESSAGE_LENGTH,Q_FIFO|Q_SHARED)){
                        DEBUG("%s already exists, binding\n",io.input_strings[i]);
                        if(rt_queue_bind(io.input_queues+i,io.input_strings[i],TM_INFINITE))
                                ERROR("Failed to create bind to queue %s!\n",io.input_strings[i]);
                }
                rt_queue_write(io.input_queues+i,&init_value,sizeof(init_value),Q_URGENT);
        }

        io.output_queues=(RT_QUEUE*)safe_malloc(io.output_num*sizeof(RT_QUEUE));
        for(i=0;i<io.output_num;i++){

                if(rt_queue_create(io.output_queues+i,io.output_strings[i],
                QUEUE_SIZE,MAX_MESSAGE_LENGTH,Q_FIFO|Q_SHARED)){
                        DEBUG("%s already exists, binding\n",io.output_strings[i]);
                        if(rt_queue_bind(io.output_queues+i,io.output_strings[i],TM_INFINITE))
                                ERROR("Failed to create and bind to queue %s!\n",io.output_strings[i]);
                }
                rt_queue_write(io.output_queues+i,&init_value,sizeof(init_value),Q_URGENT);
        }


        io.output_pipes=(RT_PIPE*)safe_malloc(io.outputp_num*sizeof(RT_PIPE));
        for(i=0;i<io.outputp_num;i++){

                if(rt_pipe_create(io.output_pipes+i,io.outputp_strings[i],P_MINOR_AUTO,256))
                        ERROR("Failed to create pipe %s\n",io.outputp_strings[i]);

        }

}

void free_io(void){
        int i;

	for(i=0;i<io.input_num;i++)
		rt_queue_delete(io.input_queues+i);
        free(io.input_queues);
	DEBUG("Deleted %d input queues\n",i);

	for(i=0;i<io.output_num;i++)
		rt_queue_delete(io.output_queues+i);
        free(io.output_queues);
	DEBUG("Deleted %d output queues\n",i);

        free(io.output_pipes);
}


void read_inputs()
{
        short i;
	Matrix sample;

        for(i=0;i<io.input_num;i++){
                switch(rt_queue_read(io.input_queues+i,&sample,sizeof(sample),1000000000)){
                //switch(rt_queue_read(io.input_queues+i,&sample,sizeof(sample),TM_INFINITE)){

		case -EINVAL:
			/*
			 * This should happen when rt_queue_read times out. So we change
			 * running to exit gracefuly
			 */
			DEBUG("%s queue does not exist! -EINVAL\n",io.input_strings[i]);
			running=0;
			break;
		case -EIDRM:
			DEBUG("%s queue has already been deleted! -EIDRM\n",io.input_strings[i]);
			break;
		case -EWOULDBLOCK:
			DEBUG("%s: EWOULDBLOCK\n",io.input_strings[i]);
			break;
		case -EINTR:
			DEBUG("%s: -EINTR\n",io.input_strings[i]);
			break;
		default:
			/* DEBUG("%s: no issues\n",io.input_strings[i]); */
			io.input_result[i]=sample;
			break;

		}

	}


	/*
	 * After this read_inputs we want the settings
	 * to be "static"
	 */

	settings_lock(&gs_mtx);
}

void write_outputs(Matrix sample)
{
	settings_unlock(&gs_mtx);
	short i;
	//printf("%4.2f ->",sample);
	for(i=0;i<io.output_num;i++){

		switch(rt_queue_write(io.output_queues+i,&sample,sizeof(sample),Q_URGENT)){
		//switch(rt_queue_write(io.output_queues+i,&sample,sizeof(sample),Q_NORMAL|Q_BROADCAST)){
			case -EINVAL:
				DEBUG("%s queue does not exist!\n",io.output_strings[i]);
				break;
			case -EIDRM:
				DEBUG("%s queue has already been deleted !\n",io.output_strings[i]);
				break;
			case -ENOMEM:
				//DEBUG("%s queue is full!\n",io.output_strings[i]);
				break;
			default:
				/* DEBUG("%s written ok\n",io.output_strings[i]); */
				break;
			}

		//	printf(" %s",io.output_strings[i]);
		}
        //printf("\n");
        for(i=0;i<io.outputp_num;i++){
                //rt_pipe_flush(io.output_pipes+i,XNPIPE_OFLUSH);
                rt_pipe_write(io.output_pipes+i, &sample, sizeof(sample), P_URGENT);
        }


}

int register_mutex(RT_MUTEX* mut, char* name){

        DEBUG("trying to create and bind %s\n",name);

        switch (rt_mutex_create(mut,name)){
                case 0://sucess!
                        DEBUG("Mutex creation succeeded!\n");
                        break;
                case -EEXIST:
                        DEBUG("EEXIST: Failed to create mutex, \n");
                        if(rt_mutex_bind(mut,name,TM_INFINITE))
                                RETERROR("binding to %s failed\n",name);

			DEBUG("bind succeed\n");
                        break;
                case -EPERM:
                        RETERROR("EPERM: Invalid context for calling mutex\n");
		case -ENOMEM:
                        RETERROR("Creation said -ENOMEM\n");
		case -EINVAL:
                        RETERROR("Got the errorr, it's -EINVAL!!\n");
		default:
                        RETERROR("Error in creation!\n");
			break;
        }
        return 0;
}

void free_mutex(RT_MUTEX* mut){
        rt_mutex_unbind(mut);
        //rt_mutex_delete(mut);

}


void* create_shm(RT_HEAP* heap, char* heap_name, size_t size, int* create){
        void *pointer;

        if(rt_heap_create(heap,heap_name,size,H_SHARED)){
                DEBUG("%s already exists, trying to bind\n",heap_name);
		if(rt_heap_bind(heap,heap_name,TM_NONBLOCK))
			ERROR("failed to create/bind to shm!\n");
		else{
			DEBUG("binding suceeded");
			*create=0;
		}
	}
	else{
		*create=1;
		DEBUG("created shm %s\n",heap_name);

	}
        
	if(rt_heap_alloc(heap,size,TM_NONBLOCK,&pointer))
		ERROR("allocation failed!\n");

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
	char buf[CHAR_BUFFER_SIZE];
	int i;

	for(i=0;*heap_name!='\0';i++,heap_name++){
		if(*heap_name!='/')
			buf[i]=*heap_name;
		else
			buf[i]='-';
	}
	buf[i]='\0';
    
	//try to bind to heap
	if(rt_heap_bind(heap,buf,TM_NONBLOCK))
	error=1;
	else{
	if(rt_heap_alloc(heap,0,TM_NONBLOCK,&pointer))
	    error=1;
	}

	//if errors occured, 
	if(error){
            DEBUG("Going for malloc\n");
            return safe_malloc(size);
	}
	else{
            DEBUG("No errors, got heap %s\n",buf);
            return pointer;
	}

}


void unbind_shm(RT_HEAP* heap){

	rt_heap_unbind(heap);

}

/*
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
*/
void get_matrix(char* section, char* key, Matrix* M1){
        char buf[CHAR_BUFFER_SIZE];

	get_string(section,key,buf);
	
	//*M1=new_matrix(buf);
	if(new_matrix_safe(M1,buf))
		ERROR("Failed to parse matrix %s from [%s]\n",key,section);
}

void store_matrix(char* section, char* key, Matrix* M1){
	char buf[CHAR_BUFFER_SIZE];

	
	matrix_string(M1,buf);
	store_string(section,key,buf);

	return;
}

void get_string(char* section, char* key, char* str){
        char buf[CHAR_BUFFER_SIZE];

        if(!settings_get(settings,section,key,buf,sizeof(buf)))
                DEBUG("Did not find %s!\n",key);
        sprintf(str,"%s",buf);

}

void store_string(char* section, char* key, char* str){
        settings_set(settings, section, key, str);
}

void get_double(char* section, char* key, double* value){
        char buf[CHAR_BUFFER_SIZE];

        if(!settings_get(settings,section,key,buf,sizeof(buf)))
                DEBUG("Did not find %s!\n",key);
        *value=atof(buf);

}

void store_double(char* section, char* key, double value){
        char buf[CHAR_BUFFER_SIZE];

        sprintf(buf,"%4.2f",value);
        settings_set(settings, section, key, buf);
}

void get_int(char* section, char* key, int* value){
        char buf[CHAR_BUFFER_SIZE];

        if(!settings_get(settings,section,key,buf,sizeof(buf)))
                DEBUG("Did not find %s!\n",key);
        *value=atoi(buf);

}

void store_int(char* section, char* key, int value){
        char buf[CHAR_BUFFER_SIZE];

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
                        DEBUG("error -EINVAL\n");
			break;
		case -EIDRM:
                        DEBUG("error -EIDRM\n");
			break;
		case -EWOULDBLOCK:
                        DEBUG("error -EWOULDBLOCK\n");
			break;
		case -EINTR:
                        DEBUG("error -EINTR\n");
			break;
		case -ETIMEDOUT:
                        DEBUG("error -ETIMEDOUT\n");
			break;
		case -EPERM:
                        DEBUG("error -EPERM\n");
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

int load_settings(char * config_file,size_t size){
        FILE *f;
        char buf[CHAR_BUFFER_SIZE];
	int i=0;

	DEBUG("config_file is %s\n",config_file);
	//Open configuration file
        sprintf(buf,"%s%s%s%s",getenv("HOME"),"/.xenomailab/workspace/",config_file,".conf");
        f=fopen(buf, "r");
        if (f==NULL)
                RETERROR("Failed to open %s for read!\n",buf);

	//Open Settings* settings based on this file, and close file
        settings = settings_open(f);
        fclose(f);
        if (settings==NULL)
                RETERROR("Failed to open settings!\n");


	//Heap name can't have /. So Untitled/sig_gen
	//becomes Untitled-sig_gen and we pass that to create_shm
	for(i=0;*config_file!='\0';i++,config_file++){
		if(*config_file!='/')
			buf[i]=*config_file;
		else
			buf[i]='-';
	}
	buf[i]='\0';

	//Allocate global_settings struct. This can either create a new shm
	//or bind to an existing shm if one by the same name already exists.
        gs=(struct global_settings*)create_shm(&settings_heap,buf,size,&settings_owner);

	//Append -m to buf to create a unique name for the mutex
	//Was Untitled-sig_gen, now is Untitled-sig_gen-m
	buf[i]='-';
	i++;
	buf[i]='m';
	i++;
	buf[i]='\0';

        if(register_mutex(&gs_mtx,buf))
            RETERROR("register_mutex has failed");

	//We either created a shm, or binded.
	//If we created, gs is empty and we need to load it
	//If not, gs already has info and we skip this step
	if(settings_owner){
		settings_lock(&gs_mtx);
	        load_gs(config_file);
		settings_unlock(&gs_mtx);
	}

        DEBUG("finished load_settings\n");
	return 0;

}

int update_settings(char* config_file){
	FILE *f;
	char buf[CHAR_BUFFER_SIZE];

	sprintf(buf,"%s%s%s%s",getenv("HOME"),"/.xenomailab/workspace/",config_file,".conf");
        DEBUG("going to open file %s for write\n",buf);
	f = fopen(buf, "w");
	if (f == NULL)
            RETERROR("failed to open %s for write!\n",buf);

	settings_lock(&gs_mtx);
	
	unload_gs();

	settings_unlock(&gs_mtx);

        if(!settings_save(settings, f))
            RETERROR("did not save settings!\n");

	fclose(f);
	
	return 0;
}

int am_alone(char* heap_name){
        int num=0,num1=0,num2=0,noWarning;;
	FILE* fd;
	char buf[CHAR_BUFFER_SIZE];

	sprintf(buf,"/proc/xenomai/registry/native/heaps/%s",heap_name);

	fd = fopen(buf,"r");
        noWarning=fscanf(fd,"type=shared:size=%d:used=%d:numaps=%d\n",&num1,&num2,&num);

	if(num<=1)
		return 1;
	else
		return 0;

}

int save_settings(char* config_file){
	int noWarning;

        if(update_settings(config_file))
            RETERROR("update_settings has failed\n");

        settings_delete(settings);

	char buf[100];

	if(settings_owner){
		DEBUG("I'm the owner, so I'm deleting the data structures\n");
		
                //If this is called from settings, block name is 0
                if(io.block_name!=0){
                    sprintf(buf,"killall %s_settings",io.block_name);
                    noWarning=system(buf);
                }

                delete_shm(&settings_heap,gs);
        	rt_mutex_delete(&gs_mtx);

	}
	else{
		DEBUG("I'm not the owner, I'm just gonna unbind\n");
                unbind_shm(&settings_heap);
        	rt_mutex_unbind(&gs_mtx);
	}

        return 0;
}


void get_task_priority(int* value){

	get_int("Task","Priority",value);

}

void stop(int signum){

	DEBUG("\n");
        DEBUG("Caught signal %d, exiting\n", signum);
        running=0;
}
