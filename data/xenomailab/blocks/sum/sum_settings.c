#include "sum_settings.h"

struct global_settings* gs;
RT_MUTEX gs_mtx;

/*
 * Load custom settings into global settings structure
 */

void load_gs(void){

	get_double("Operation","OutputGain",&gs->OutputGain);
	get_double("Operation","Ch0Gain",&gs->Ch0Gain);
	get_double("Operation","Ch1Gain",&gs->Ch1Gain);
	get_double("Operation","Ch2Gain",&gs->Ch2Gain);

	get_int("Task","Priority",&gs->task_prio);
}

/*
 * Unload global settings structure to settings file holder
 */

void unload_gs(void){

	store_double("Operation", "OutputGain", gs->OutputGain);
	store_double("Operation", "Ch0Gain", gs->Ch0Gain);
	store_double("Operation", "Ch1Gain", gs->Ch1Gain);
	store_double("Operation", "Ch2Gain", gs->Ch2Gain);

	store_int("Task","Priority",gs->task_prio);
}
