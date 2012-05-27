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
#include "lpt.h"
#include "AD_cart_settings.h"

#define DELAY 15000

Matrix periodic_function(Matrix* inputChannel,short numChannels){
	Matrix ret=empty_matrix(1,1);
	int sample1=0,sample2=0,sample_pendulo=0;
	double sample;
	static short size=sizeof(int)*8;

	lpt_lock();
		clear_pin(SEL);
		clear_pin(OE2);

		rt_task_sleep(DELAY);

		sample1=pin_is_set(DATA_PINS());
		set_pin(SEL);

		rt_task_sleep(DELAY);

		sample2=pin_is_set(DATA_PINS());
		set_pin(OE2);
	lpt_unlock();
		
		sample1=sample1>>12;
		sample2=sample2>>12;

		sample_pendulo=sample2+(sample1<<8);
		//convert to signed!
		sample_pendulo=(sample_pendulo<<(size/2))>>(size/2);
	
		sample=(double)sample_pendulo;

		ret.matrix[0][0]=sample;

	return ret;
}

void loop(void *arg){
	Matrix outputMatrix;

	lpt_init();

	while (running) {
		read_inputs();
		
		outputMatrix=periodic_function(io.input_result,io.input_num);

		write_outputs(outputMatrix);

	}

	lpt_close();
}

int main(int argc, char* argv[]){

	initialize_block(argc,argv,sizeof(struct global_settings),1,0);

	start_task(gs->task_prio,&loop);

	wait_for_task_end();
	
	finalize_block();

	return 0;
}
