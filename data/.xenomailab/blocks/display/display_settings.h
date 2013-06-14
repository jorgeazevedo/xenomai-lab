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

#ifndef _DISPLAY_SETTINGS_H_
#define _DISPLAY_SETTINGS_H_

#include "nrt_block_io.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct global_settings{
        char output[25];
};

/*
 * gs_mtx ensures mutual exclusion
 * on the gs structure from concurrent
 * access from settings GUI
 */
extern RT_MUTEX gs_mtx;
extern struct global_settings* gs;

void load_gs(void);	//block specific
void unload_gs(void);	//block specific

#ifdef __cplusplus
}
#endif

#endif
