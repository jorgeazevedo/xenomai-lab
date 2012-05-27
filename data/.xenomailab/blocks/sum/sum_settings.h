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


#ifndef _SUM_SETTINGS_H_
#define _SUM_SETTINGS_H_

#include "rt_block_io.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct global_settings{

        double OutputGain;
	double Ch0Gain;
	double Ch1Gain;
	double Ch2Gain;

        int task_prio;//Real time task priority (0-99, higher is greater)
};

extern struct global_settings* gs;

void load_gs(void);	//block specific
void unload_gs(void);	//block specific

#ifdef __cplusplus
}
#endif

#endif
