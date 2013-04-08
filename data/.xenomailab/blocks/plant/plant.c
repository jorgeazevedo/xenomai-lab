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

#include "plant_settings.h"

Matrix system_block(Matrix B,Matrix A,Matrix u){
	static double uA[CMAX];
	static double yA[CMAX];
	int i;
	double aux=0;
	Matrix yo=empty_matrix(1,1);

	for(i=0;i<B.columns-1;i++){
	uA[i]=uA[i+1];
	}
	uA[B.columns-1]=u.matrix[0][0];

	for(i=0;i<B.columns;i++){
	aux=aux+B.matrix[0][i]*uA[B.columns-1-i];
	}
	for(i=0;i<A.columns-1;i++){
	aux=aux-A.matrix[0][i+1]*yA[A.columns-2-i];
	}

	for(i=0;i<A.columns-2;i++){
	yA[i]=yA[i+1];
	}
	yA[A.columns-2]=aux;

	yo.matrix[0][0]=aux;
	return yo;
}

void init() {
}

Matrix transfer_function(Matrix* input_channel,short num_channels) {

	Matrix ret = empty_matrix(1,1);

        ret = system_block(gs->B,gs->A,input_channel[0]);

	return ret;
}

void cleanup() {
}

STD_BLOCK_MAIN()
