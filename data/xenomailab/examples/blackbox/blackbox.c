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
#include "blackbox.h"
#include "rt_block_io.h"

RT_MUTEX blackbox_mutex;

int blackbox_read(){
	int sample=0;

	blackbox_lock();
		sample=adc12bpp();
	blackbox_unlock();

	return sample;
}

void blackbox_write(unsigned int sample){

	blackbox_lock();
		dac12bpp(sample);
	blackbox_unlock();
}

void blackbox_init(){

	//Mutex to control concurrent access to blackbox
	//(function from rt_block_io.h
	register_mutex(&blackbox_mutex,"BlackBoxMutex");
	
	if (ioperm(LPT,2,1))
		ERROR("Couldn't get the port at %x, are you root?\n", LPT);

}

inline void blackbox_lock(){
	rt_mutex_acquire(&blackbox_mutex,TM_INFINITE);
}

inline void blackbox_unlock(){
	rt_mutex_release(&blackbox_mutex);
}

void blackbox_close(){
	rt_mutex_delete(&blackbox_mutex);
}

//	DAC12BPP
//	Writes Data to 12BPP D/A Converter
//	Tiago Oliveira - March 1997
//	Alexandre Mota - DETUA
//	02/May/2002
//	Last Revision : 02/05/2002

void dac12bpp(unsigned int DA_Value)
{
	char aByte;
	int n;
	unsigned int temp;

	//error handling
	if ( DA_Value > 4095) DA_Value = 4095;
	if ( DA_Value < 0)  DA_Value = 0;

	
	aByte = 0x00;
	//0x22
	//100010
	outb(0x22, 0x378);

	for (n=1;n<=12;n++) {
		//*Load 12nd bit but from DA_Value
		//temp = DA_Value & 0000 1000 0000 0000
		//temp = temp= 0000 X000 0000 0000
		temp = DA_Value & 0x0800;
		//*Shift to 8 bits, load register bits
		//aByte= X000 0000 | 0010 0010
		//abyte= X010 0010
		aByte = (char)((temp >> 4) | 0x22);
		//outb(X010 0010)
		//outb(X010 0011)
		outb((aByte & 0xfe), 0x378);
		outb((aByte | 0x01), 0x378);

		DA_Value <<= 1;
	}

	//0011 0010
	outb(0x32, 0x378);

	return;
}

//	ADC12BPP
//	Reads Data from the 12BPP A/D converter.
//	Calling syntax : AD_Value = adc12bpp()
//	0 <= value <= 4095
//	-1 - Error !
//	Tiago Oliveira - March 1997
//	Alexandre Manuel Mota
//	02/May/2002
//	Last Revision : 02/05/2002

int adc12bpp(void) 
{
	int n;
	unsigned int bitn;
	unsigned int AD_Value;
	unsigned char ppStatus;
	unsigned int timeOut_Cnt;
	
	AD_Value = 0;
	outb(0xFC, 0x378);
	timeOut_Cnt = 0;
	do {
		if ((timeOut_Cnt++) >= 10000) {
			return(-1);
		}
		ppStatus = inb(0x379);
	} while (!((~ppStatus) & 0x08));
	outb(0xFD, 0x378);
	outb(0xFC, 0x378);
	for(n=0;n<=11;n++) {
		outb(0xFD, 0x378);
		ppStatus = inb(0x379);
		if (((~ppStatus) & 0x08)==0) bitn = 0;
		else bitn = 1;
		AD_Value = (AD_Value <<= 1) | bitn;
		outb(0xFC, 0x378);
	}
	outb(0xFE, 0x378);
	return((int)AD_Value);
}
