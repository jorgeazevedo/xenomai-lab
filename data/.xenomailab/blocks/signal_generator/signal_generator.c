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
#include <math.h>

#include "signal_generator_settings.h"

long* current_period;
RT_HEAP sampling_heap;

void init() {
	current_period=bind_shm(&sampling_heap,"tickPeriod",sizeof(long));
	
	DEBUG("Outputing a %s wave of amplitude %4.2f, freq %4.2f, duty %4.2f, dc %4.2f\n",gs->wave,gs->wave_amp,gs->wave_freq,gs->wave_duty,gs->wave_dc);
}

Matrix transfer_function(Matrix* input_channel,short num_channels) {
	Matrix ret=empty_matrix(1,1);
	double sample;
	static int n=0;
	long period=*current_period;
	int halfpointx;
	float slope;

	switch(gs->wave[0]){
		case 'S': case 's':
			sample=sinf(2*M_PI*gs->wave_freq*n*period*pow(10,-9))*gs->wave_amp/2+gs->wave_dc;
			break;
		case 'T': case 't':
			halfpointx=0.5/(gs->wave_freq*period*pow(10,-9));
			slope=gs->wave_amp/halfpointx;
			if(n<=halfpointx)
				sample=slope*n-gs->wave_amp/2;
			else
				sample=-slope*n+(3*gs->wave_amp/2);

			//wrap around
			if(n>(2*halfpointx))
				n=-1;

			break;
		case 'P': case 'p':
			if(n<=(gs->wave_duty*0.01)/(gs->wave_freq*period*pow(10,-9)))
				sample=gs->wave_dc+gs->wave_amp/2;
			else
				sample=gs->wave_dc-gs->wave_amp/2;
			//wrap around
			if(n>(1/(gs->wave_freq*period*pow(10,-9))))
				n=-1;

			break;
	}
	n++;

	ret.matrix[0][0]=sample;

	return ret;
}

void cleanup() {
	unbind_shm(&sampling_heap);
}

STD_BLOCK_MAIN()
