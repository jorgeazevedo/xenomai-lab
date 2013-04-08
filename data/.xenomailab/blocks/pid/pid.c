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

void init() {
}

Matrix transfer_function(Matrix* input_channel,short num_channels) {

	Matrix ret = empty_matrix(1,1);

        ret.matrix[0][0] = pid(input_channel[0].matrix[0][0], gs->kp, gs->td, gs->ki);

	return ret;
}

void cleanup() {
}

STD_BLOCK_MAIN()
