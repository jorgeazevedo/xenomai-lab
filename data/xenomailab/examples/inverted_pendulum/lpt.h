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

#ifndef _LPT_H_
#define _LPT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <parapin.h>
#include <native/mutex.h>

//Pin out
//INPUT
#define D0 LP_PIN13
#define D1 LP_PIN12
#define D2 LP_PIN10
#define D3 LP_PIN11
#define D4 LP_PIN01
#define D5 LP_PIN14
#define D6 LP_PIN16
#define D7 LP_PIN17
#define DATA_PINS() D0|D1|D2|D3|D4|D5|D6|D7
//OUTPUT
#define SEL LP_PIN02
#define OE1 LP_PIN03 //pendulo
#define OE2 LP_PIN04 //car
#define DIRE LP_PIN05
#define PWM LP_PIN06

extern RT_MUTEX lpt_mutex;

void lpt_init();
void lpt_close();

inline void lpt_lock();
inline void lpt_unlock();

inline void lpt_delay();

#ifdef __cplusplus
}
#endif

#endif
