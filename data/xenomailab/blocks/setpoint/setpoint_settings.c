#include "setpoint_settings.h"

struct global_settings* gs;
RT_MUTEX gs_mtx;

/*
 * Load custom settings into global settings structure
 */

void load_gs(void){

	get_double("Operation","Setpoint",&gs->setpoint);

	get_int("Task","Priority",&gs->task_prio);
}

/*
 * Unload global settings structure to settings file holder
 */

void unload_gs(void){

	store_double("Operation", "Setpoint", gs->setpoint);

	store_int("Task","Priority",gs->task_prio);
}
