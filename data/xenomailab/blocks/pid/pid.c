#include <stdio.h>

#include "pid_settings.h"

double pid(double input, double Kp, double Td, double Ki)
{
        static  float   input_1=0,it_1=0;
        int     umax=100,umin=-100;
        float   ut,pt,dt,it;

        //P
        pt = Kp * input;

        //I
        it = it_1 + Ki* input;
        if (it > umax) it=umax;
        if (it < umin) it=umin;
        it_1 = it;

        //D
        dt = Td * ( input - input_1);


        //Output
        ut =pt + it + dt;

        //cliping of output
        if (ut > umax) ut=umax;
        if (ut < umin) ut=umin;

        //keep this for D
        input_1 = input;

        return ut;
}

Matrix periodic_function(Matrix* inputChannel,short numChannels){
	Matrix ret;

        ret.matrix[0][0]=pid(inputChannel[0].matrix[0][0],gs->kp,gs->td,gs->ki);

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
