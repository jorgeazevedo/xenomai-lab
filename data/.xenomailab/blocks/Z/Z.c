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

#include "Z_settings.h"

void init() {
	Matrix init_value = new_matrix("[8]");
	int i;
	int delays = gs->N;

	for(i=0;i < delays;i++)
		write_output_queues(&init_value);
}

Matrix transfer_function(Matrix* input_channel,short num_channels) {

	return *input_channel;
}

void cleanup() {
}

STD_BLOCK_MAIN()
