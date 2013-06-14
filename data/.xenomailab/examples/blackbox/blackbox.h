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

#ifndef _BLACKBOX_H_
#define _BLACKBOX_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <sys/io.h>
#include <native/mutex.h>
#define LPT 0x378

extern RT_MUTEX blackbox_mutex;

int adc12bpp(void);
void dac12bpp(unsigned int DA_Value);
void blackbox_init();
void blackbox_close();

inline void blackbox_lock();
inline void blackbox_unlock();

#ifdef __cplusplus
}
#endif

#endif
