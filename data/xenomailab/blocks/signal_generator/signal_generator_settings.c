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

#include "signal_generator_settings.h"	//block specific

struct global_settings* gs;
RT_MUTEX gs_mtx;

/*
 * Load custom settings into global settings structure
 */

void load_gs(void){	//block specific

	get_string("Operation","Wave",gs->wave);

	get_double("Operation","DC",&gs->wave_dc);
	get_double("Operation","Amplitude",&gs->wave_amp);
	get_double("Operation","Frequency",&gs->wave_freq);
	get_double("Operation","DutyCycle",&gs->wave_duty);

	get_int("Task","Priority",&gs->task_prio);
}

/*
 * Unload global settings structure to settings file holder
 */

void unload_gs(void){	//block specific

	store_string("Operation", "Wave", gs->wave);

	store_double("Operation", "DC", gs->wave_dc);
	store_double("Operation", "Amplitude", gs->wave_amp);
	store_double("Operation", "Frequency", gs->wave_freq);
	store_double("Operation", "DutyCycle", gs->wave_duty);

	store_int("Task","Priority",gs->task_prio);
}
