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

#include "plant_settings.h"

Matrix systemBlock(Matrix A,Matrix B,Matrix u){
    static double uA[CMAX];
    static double yA[CMAX];
    int i;
    double aux=0;
    Matrix yo=empty_matrix(1,1);

    for(i=0;i<A.columns-1;i++){
        uA[i]=uA[i+1];
    }
    uA[A.columns-1]=u.matrix[0][0];

    for(i=0;i<A.columns;i++){
        aux=aux+A.matrix[0][i]*uA[A.columns-1-i];
    }
    for(i=0;i<B.columns-1;i++){
        aux=aux-B.matrix[0][i+1]*yA[B.columns-2-i];
    }

    for(i=0;i<B.columns-2;i++){
        yA[i]=yA[i+1];
    }
    yA[B.columns-2]=aux;

    yo.matrix[0][0]=aux;
    return yo;
}

Matrix periodic_function(Matrix* inputChannel,short numChannels){
	Matrix ret=empty_matrix(1,1);

	/*
	 * Insert periodic code here.
	 */
	
	//Default operation: apply -1 gain
	//ret=matrix_mul_double(&inputChannel[0], -2);
	DEBUG("A\n");
	matrix_print(&gs->A);
	DEBUG("B\n");
	matrix_print(&gs->B);

        ret=systemBlock(gs->A,gs->B,inputChannel[0]);

	return ret;
}

void loop(void *arg){
	Matrix outputMatrix=empty_matrix(1,1);

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
