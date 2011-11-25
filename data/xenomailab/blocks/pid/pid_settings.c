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
