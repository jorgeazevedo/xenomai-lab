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

#include "tick_settings.h"

Matrix periodic_function(Matrix* inputChannel,short numChannels){
	Matrix retMatrix;
	static double value=4.0;

	value++;
	retMatrix.matrix[0][0]=value;


	return retMatrix;
}

void loop(void *arg){
	RT_HEAP sampling_heap;
	Matrix outputMatrix=new_matrix("[4]");
	long* current_period;
	int create;

	current_period=(long*) create_shm(&sampling_heap,"tickPeriod",sizeof(long),&create);

	*current_period=gs->sampling_period*1000;
	rt_task_set_periodic(NULL, TM_NOW,*current_period);

	DEBUG("Ticking with period %ld\n",*current_period);

	while (running) {
		// DEBUG("Ticking with period %ld\n",*current_period);
		rt_task_wait_period(NULL);
		settings_lock(&gs_mtx);

		outputMatrix=periodic_function(io.input_result,io.input_num);

		write_outputs(outputMatrix);
		if(*current_period==0)
			break;
		if(*current_period!=gs->sampling_period*1000){
			*current_period=gs->sampling_period*1000;
			rt_task_set_periodic(NULL, TM_NOW,*current_period);
		}
	}

	delete_shm(&sampling_heap,current_period);
}

int main(int argc, char* argv[]){

	initialize_block(argc,argv,sizeof(struct global_settings),1,0);

	start_task(gs->task_prio,&loop);

	wait_for_task_end();
	
	finalize_block();

	return 0;
}
