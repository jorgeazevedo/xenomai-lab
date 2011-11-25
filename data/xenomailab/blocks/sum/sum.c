#include <stdio.h>

#include "sum_settings.h"

Matrix periodic_function(Matrix* inputChannel,short numChannels){
	Matrix ret=empty_matrix(1,1);
	Matrix temp=empty_matrix(1,1);

	temp=matrix_mul_double(&inputChannel[0], gs->Ch0Gain);
	ret=matrix_sum(&ret,&temp);

	temp=matrix_mul_double(&inputChannel[1], gs->Ch1Gain);
	ret=matrix_sum(&ret,&temp);

	if(numChannels>2){
		temp=matrix_mul_double(&inputChannel[2], gs->Ch2Gain);
		ret=matrix_sum(&ret,&temp);
	}

	ret=matrix_mul_double(&ret,gs->OutputGain);

	return ret;
}

void loop(void *arg){
	Matrix outputMatrix;

	/*
	 * Insert initialization code here.
	 * e.g. open a file.
	 */

	while (running) {
		read_inputs();
		
		outputMatrix=periodic_function(io.input_result,io.input_num);

		write_outputs(outputMatrix);

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
