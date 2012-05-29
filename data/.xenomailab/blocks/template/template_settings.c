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

#include "template_settings.h"

struct global_settings* gs;
RT_MUTEX gs_mtx;

/*
 * Load custom settings into global settings structure
 */

void load_gs(void){

	/*
	 * Change this code.
	 */
	get_string("Operation","aString",gs->aString);
	get_int("Operation","anInt",&gs->anInt);
	get_double("Operation","aDouble",&gs->aDouble);
	get_matrix("Operation","aMatrix",&gs->aMatrix);

	get_int("Task","Priority",&gs->task_prio);
}

/*
 * Unload global settings structure to settings file holder
 */

void unload_gs(void){

	/*
	 * Change this code.
	 */
	store_string("Operation", "aString", gs->aString);
	store_int("Operation", "anInt", gs->anInt);
	store_double("Operation", "aDouble", gs->aDouble);
	store_matrix("Operation","aMatrix",&gs->aMatrix);

	store_int("Task","Priority",gs->task_prio);
}
