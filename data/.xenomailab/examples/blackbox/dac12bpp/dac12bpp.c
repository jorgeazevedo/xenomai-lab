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

#include "blackbox.h"
#include "dac12bpp_settings.h"

Matrix periodic_function(Matrix* inputChannel,short numChannels){

	//Get value
	int value=(int)inputChannel[0].matrix[0][0];

	//Saturate
	if(value<0)
		value=0;
	if(value>4095)
		value=4095;

	//Write value
	blackbox_write(value);

	return inputChannel[0];
}

void loop(void *arg){
	Matrix outputMatrix=empty_matrix(1,1);
	int i=0;

	blackbox_init();

	while (running) {
		read_inputs();
		
		outputMatrix=periodic_function(io.input_result,io.input_num);
	
		write_outputs(outputMatrix);
	}

	blackbox_close();
}

int main(int argc, char* argv[]){

	initialize_block(argc,argv,sizeof(struct global_settings),1,0);

	start_task(gs->task_prio,&loop);

	wait_for_task_end();
	
	finalize_block();

	return 0;
}
