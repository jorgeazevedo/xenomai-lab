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

#include "sum_settings.h"

void init() {
}

Matrix transfer_function(Matrix* input_channel,short num_channels) {
	Matrix ret=empty_matrix(1,1);
	Matrix temp=empty_matrix(1,1);
	int i=0;

	temp=matrix_mul_double(&input_channel[0], gs->Ch0Gain);
	ret=matrix_sum(&ret,&temp);

	if(num_channels>=2){
		temp=matrix_mul_double(&input_channel[1], gs->Ch1Gain);
		ret=matrix_sum(&ret,&temp);

		if(num_channels>=3){
			temp=matrix_mul_double(&input_channel[2], gs->Ch2Gain);
			ret=matrix_sum(&ret,&temp);

			for(i=3;i<num_channels;i++){
				ret=matrix_sum(&ret,&input_channel[i]);
			}
		}
	}

	ret=matrix_mul_double(&ret,gs->OutputGain);

	return ret;
}

void cleanup() {
}

STD_BLOCK_MAIN()
