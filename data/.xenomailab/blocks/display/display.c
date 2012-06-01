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

#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//#include <native/pipe.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

#include <string.h>

#include "display_settings.h"	//block specific
#include "nrt_block_io.h" //Contais ERROR and DEBUG macros

#define ROUND(x) ((x)>=0?(int)((x)+0.5):(int)((x)-0.5))

int my_itoa(int val, char* buf);

void print_num(double num){
	char buf[32];

	int in=ROUND(num);

	switch(gs->output[0]){
		case 'F': case 'f':
			printf("%4.2f",num);
			break;
		case 'I': case 'i':
			printf("%d",in);
			break;
		case 'H': case 'h':
			printf("%X",in);
			break;
		case 'B': case 'b':
			my_itoa(in,buf);
			printf("%s",buf);
			break;
	}
}

Matrix periodic_function(Matrix* inputChannel,short numChannels){
	int i,j,k;
	printf("\n");
	for(i=0;i<numChannels;i++){
		DEBUG("%s:\n",io.inputp_strings[i]+36);
		matrix_print(&inputChannel[i]);
			
	}
	
/*
	for(i=0;i<io.inputp_num;i++){
		dou=inputChannel[i];
		in=ROUND(dou);
		my_itoa(in,buf);

		switch(gs->output[0]){
			case 'F': case 'f':
		printf("%s:\t%4.2f\t",io.inputp_strings[i]+36,dou);
				break;
			case 'I': case 'i':
		printf("%s:\t%d\t",io.inputp_strings[i]+36,in);
				break;
			case 'H': case 'h':
		printf("%s:\t%X\t",io.inputp_strings[i]+36,in);
				break;
			case 'B': case 'b':
		printf("%s:\t%s\t",io.inputp_strings[i]+36,buf);
				break;
		}

	}
	printf("\n");
	*/
}

void loop(void){
	Matrix outputMatrix;

	DEBUG("Loop start\n");
	//This fflush is important! Without it  before the loop, interface will fail!
	fflush(stdout);
	while(stopflag){
		read_inputs();

		outputMatrix=periodic_function(io.inputp_result,io.inputp_num);

		write_outputs(outputMatrix);
	}


}

int main (int argc, char *argv[]){

	parse_args(argc,argv);

	DEBUG("%s started\n",argv[0]);

	assert_io_min(1,0);

	//On SIGINT (Ctrl-C), SIGTERM (kill pid) call stop() (from blockio.h)
	signal(SIGTERM, stop);
	signal(SIGINT, stop);

	create_io();

	load_settings(io.config_file,sizeof(struct global_settings),0);

	loop();
	
	DEBUG("Terminated\n");

	free_args();
	free_io();

	save_settings(io.config_file,0);

	return 0;
}

int my_itoa(int val, char* buf)
{
	const unsigned int radix = 2; 

	char* p;
	unsigned int a;        //every digit
	int len;
	char* b;            //start of the digit char
	char temp;
	unsigned int u;

	p = buf;

	if (val < 0){
		int offset=pow(2,sizeof(int));
		/* *p++ = '-'; */
		/* val = 0 - val; */
		val=offset+val;
	}
	u = (unsigned int)val;

	b = p;

	do{
		a = u % radix;
		u /= radix;

		*p++ = a + '0';

	} while (u > 0);

	len = (int)(p - buf);

	*p-- = 0;

	//swap
	do{
		temp = *p;
		*p = *b;
		*b = temp;
		--p;
		++b;

	} while (b < p);

	return len;
}
