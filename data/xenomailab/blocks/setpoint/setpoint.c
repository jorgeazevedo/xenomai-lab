#include <stdio.h>

#include "setpoint_settings.h"

Matrix periodic_function(Matrix* inputChannel,short numChannels){
	Matrix ret=empty_matrix(1,1);

	ret.matrix[0][0]=gs->setpoint;

	return ret;
}

void loop(void *arg){
	Matrix outputMatrix;

	while (running) {
		read_inputs();
		
		outputMatrix=periodic_function(io.input_result,io.input_num);

		write_outputs(outputMatrix);

	}
}

int main(int argc, char* argv[]){

	initialize_block(argc,argv,sizeof(struct global_settings),1,0);

	start_task(gs->task_prio,&loop);

	wait_for_task_end();
	
	finalize_block();

	return 0;
}
