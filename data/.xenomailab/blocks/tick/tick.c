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

//No period bellow 100us
#define SAFEZONE 100000

void loop(void *arg){
	RT_HEAP sampling_heap;
	Matrix outputMatrix=empty_matrix(1,1);
	long* current_period;
	int create;

	//This works as a global variable
	current_period=(long*) create_shm(&sampling_heap,"tickPeriod",sizeof(long),&create);

	*current_period=gs->sampling_period*1000;

	if(*current_period<SAFEZONE)
		*current_period=SAFEZONE;

	rt_task_set_periodic(NULL, TM_NOW,*current_period);

	while(running){
		rt_task_wait_period(NULL);

		write_outputs(outputMatrix);

		//Change period if changed in GUI
		if(*current_period!=gs->sampling_period*1000){
			*current_period=gs->sampling_period*1000;
			if(*current_period<SAFEZONE)
				*current_period=SAFEZONE;
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
