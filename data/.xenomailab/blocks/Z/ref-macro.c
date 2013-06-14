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

#include <stdio.h>

#include "Z_settings.h"

void init() {
	Matrix init_value = new_matrix("[8]");
	int i;

	for(i=0;i < gs->N;i++)
		write_output_queues(&init_value);
}

Matrix transfer_function(Matrix* input_channel,short num_channels){

	return *input_channel;
}

void cleanup() {
}

///////////////////////////////////////////////////////////////////////////////
//Everything bellow this point should be in rt_block_io.h
//
//TODO I don't know if loop() can see init, cleanup and transferFunction like
//     that
//TODO It might also be interesting to have io.input_result be a NULL
//     terminated array, so we can not pass io.input_num to transferFunction
///////////////////////////////////////////////////////////////////////////////

#define BLOCK_MAIN() \
int main(int argc, char* argv[]){ \
	block_main(argc,argv,sizeof(struct global_settings),&loop); \
	return 0; \
} \

void loop(void *arg){

	init();

	while (running) {
		read_inputs();
		
		write_outputs(transfer_function(io.input_result,io.input_num));
	}

	cleanup();
}

void block_main(int argc, char* argv[], size_t settings_size, void* task_function){
	initialize_block(argc,argv,settings_size,1,0);

	start_task(gs->task_prio,task_function);

	wait_for_task_end();
	
	finalize_block();

}

BLOCK_MAIN()

