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


#include "pid_settings.h"

struct global_settings* gs;
RT_MUTEX gs_mtx;

/*
 * Load custom settings into global settings structure
 */

void load_gs(void){

	get_double("Operation","Kp",&gs->kp);
	get_double("Operation","Td",&gs->td);
	get_double("Operation","Ki",&gs->ki);

	get_int("Task","Priority",&gs->task_prio);
}

/*
 * Unload global settings structure to settings file holder
 */

void unload_gs(void){

	store_double("Operation","Kp",gs->kp);
	store_double("Operation","Td",gs->td);
	store_double("Operation","Ki",gs->ki);

	store_int("Task","Priority",gs->task_prio);
}
