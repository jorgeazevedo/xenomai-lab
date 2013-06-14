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

#include "adc12bpp_settings.h"

struct global_settings* gs;
RT_MUTEX gs_mtx;

/*
 * Load custom settings into global settings structure
 */

void load_gs(void){

	/*
	 * Insert code here
	 *
	 * Example:
	get_string("Operation","Wave",gs->wave);
	get_double("Operation","DC",&gs->wave_dc);
	get_matrix("Operation","Am",&gs->Am);
	 */

	get_int("Task","Priority",&gs->task_prio);
}

/*
 * Unload global settings structure to settings file holder
 */

void unload_gs(void){

	/*
	 * Insert code here
	 *
	 * Example:
	store_string("Operation", "Wave", gs->wave);
	store_double("Operation", "DC", gs->wave_dc);
	store_matrix("Operation","Am",&gs->Am);
	 */

	store_int("Task","Priority",gs->task_prio);
}
