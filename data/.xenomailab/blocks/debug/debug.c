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

#include <stdio.h>

#include "debug_settings.h"

Matrix periodic_function(Matrix* inputChannel,short numChannels){
	char buf[CHAR_BUFFER_SIZE];
	struct debugframe temp;
	static unsigned long long epoch = 0;
	int i,ret=1;
	
	while(ret){
		ret = rt_queue_read(io.debug_queue,&temp,sizeof(temp),TM_NONBLOCK);
		
		//If queue has no more messages to process
		//or has been deleted and hence become invalid
		if(ret == -EWOULDBLOCK || ret == -EINVAL)
			break;
		
		if(!epoch)
			epoch = temp.start_time;

		DEBUG("**Block: %s\n",temp.block_name);

		DEBUG("start: %llu.%06llu ms\n",
		      (temp.start_time - epoch) / 1000,
		      (temp.start_time - epoch) % 1000);
		DEBUG("end  : %llu.%06llu ms\n",
		      (temp.end_time - epoch) / 1000,
		      (temp.end_time - epoch) % 1000);

		matrix_string(&temp.output, &buf);
		DEBUG("Output  : %s\n",buf);
		DEBUG("Inputs : %d\n",temp.input_num);
		for(i=0;i<temp.input_num;i++)
			matrix_print_pretty(&temp.input[i],temp.input_name[i],NULL);
	}
}

void loop(void *arg){

	/*
	 * Insert initialization code here.
	 * e.g. open a file.
	 */

	while (running) {
		read_inputs();
		
		DEBUG("***************************\n");
		DEBUG("**********TICK# %d\n",(int)io.input_result[0].matrix[0][0]);
		
		periodic_function(io.input_result,io.input_num);

		settings_unlock(&gs_mtx);

	}

	/*
	 * Insert finalization code here
	 * e.g. close a file.
	 */
}

int main(int argc, char* argv[]){

	initialize_block(argc,argv,sizeof(struct global_settings),1,0);

	start_task(gs->task_prio,&loop);

	wait_for_task_end();
	
	finalize_block();

	return 0;
}
