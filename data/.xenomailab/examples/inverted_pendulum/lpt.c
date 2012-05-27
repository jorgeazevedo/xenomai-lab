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
#include "lpt.h"
#include "rt_block_io.h"

RT_MUTEX lpt_mutex;

void lpt_init(){

	//Mutex to control concurrent access to lp
	//(function from rt_block_io.h
	register_mutex(&lpt_mutex,"LptMutex");
	
	//Protect from concurrent initialization!
	lpt_lock();

	//Parapin init
	if (pin_init_user(LPT1) < 0)
		ERROR("Couldn't initialize parapin lib at %d, are you root?\n",LPT1);
	//Pin modes
	pin_input_mode(DATA_PINS());
	pin_output_mode(LP_DATA_PINS);

	//hardware initialization
	set_pin(SEL|OE1|OE2);
	clear_pin(PWM|DIRE);
	
	lpt_unlock();
}

inline void lpt_delay(){
	rt_task_sleep(15000);
}

inline void lpt_lock(){
	rt_mutex_acquire(&lpt_mutex,TM_INFINITE);
}

inline void lpt_unlock(){
	rt_mutex_release(&lpt_mutex);
}

void lpt_close(){
	rt_mutex_delete(&lpt_mutex);
}
