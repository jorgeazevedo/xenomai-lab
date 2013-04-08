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

#include "template_settings.h"

/*
 * Insert initialization code here.
 * e.g. open a file.
 */
void init() {
}

/*
 * Insert main function here.
 */
Matrix transfer_function(Matrix* input_channel, short num_channels){
	Matrix ret = empty_matrix(1,1);
	
	//Default operation: apply -1 gain
	ret = matrix_mul_double(&input_channel[0], -1);

	return ret;
}

/*
 * Insert finalization code here
 * e.g. close a file.
 */
void cleanup() {
}


STD_BLOCK_MAIN()
