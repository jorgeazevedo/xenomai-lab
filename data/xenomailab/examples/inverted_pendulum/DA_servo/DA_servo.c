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
#include "DA_servo_settings.h"

int fakeWave=0;

void pwm(void *arg){
 	double duty_cycle=0,Ton=0;
	long current_period=0;
	int DIR;

	current_period=gs->period*1000;
	rt_task_set_periodic(NULL, TM_NOW,current_period);
	while (running) {
		rt_task_wait_period(NULL);
		
		DEBUG("Hi guys %ld, %4.2f!\n",current_period,Ton);

		duty_cycle=io.input_result[0].matrix[0][0];

		//saturate
		if(duty_cycle<-100)
			duty_cycle=-100;
		if(duty_cycle>100)
			duty_cycle=100;

		//Motor specific.
		if(duty_cycle<0){
			DIR=1;
			duty_cycle=100+duty_cycle;
		}
		else{
			DIR=0;
		}
		Ton=duty_cycle*0.01*current_period;

		lpt_lock();
		fakeWave=1;
		set_pin(PWM);
                change_pin(DIRE,DIR?LP_SET:LP_CLEAR);
		lpt_unlock();

		rt_task_sleep(Ton);

		lpt_lock();
		fakeWave=0;
		clear_pin(PWM);
		lpt_unlock();
		
		if(current_period==0)
			break;
		if(current_period!=gs->period*1000){
			current_period=gs->period*1000;
			rt_task_set_periodic(NULL, TM_NOW,current_period);
		}
	}
}

Matrix periodic_function(Matrix* inputChannel,short numChannels){
	Matrix ret=empty_matrix(1,1);

	ret.matrix[0][0]=fakeWave;

	return ret;
}

void loop(void *arg){
	Matrix outputMatrix=empty_matrix(1,1);
	RT_TASK pwm_task;

	lpt_init();

	rt_task_spawn(&pwm_task,NULL,0,gs->task_prio,0,&pwm,NULL);

	while (running) {
		read_inputs();
		
		//outputMatrix=periodic_function(io.input_result,io.input_num);

		write_outputs(outputMatrix);
	}

	rt_task_delete(&pwm_task);

	lpt_close();
}

int main(int argc, char* argv[]){

	initialize_block(argc,argv,sizeof(struct global_settings),1,0);

	start_task(gs->task_prio,&loop);

	wait_for_task_end();
	
	finalize_block();

	return 0;
}
