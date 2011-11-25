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
