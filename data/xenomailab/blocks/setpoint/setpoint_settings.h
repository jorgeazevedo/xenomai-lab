#ifndef _SETPOINT_SETTINGS_H_
#define _SETPOINT_SETTINGS_H_

#include "rt_block_io.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct global_settings{

        double setpoint;

        int task_prio;//Real time task priority (0-99, higher is greater)
};

extern struct global_settings* gs;

void load_gs(void);	//block specific
void unload_gs(void);	//block specific

#ifdef __cplusplus
}
#endif

#endif
