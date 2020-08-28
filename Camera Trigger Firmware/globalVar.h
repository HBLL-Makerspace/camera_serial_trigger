/*
 * IncFile1.h
 *
 * Created: 7/28/2020 4:57:08 PM
 *  Author: chadb
 */ 


#ifndef GLOBALVAR_H_
#define GLOBALVAR_H_
#include <stdbool.h>
#define GLOBALVAR_BRIGHTNESS_MEM_SLOT 0
#define GLOBALVAR_CONTRAST_MEM_SLOT 1
#define GLOBALVAR_RESET_MEM_SLOT 2
#define GLOBALVAR_TIMING_BEGIN_MEM_SLOT 100
#define GLOBALVAR_TIMING_SETPOINTS 99
#define GLOBALVAR_TIMING_SET_COUNT 98
#define GLOBALVAR_EN_SHUT 0
#define GLOBALVAR_EN_FOC 1
#define GLOBALVAR_DS_SHUT 2
#define GLOBALVAR_DS_FOC 3
#define GLOBALVAR_MAX_EVENTS 256

typedef void (*handler)();

static volatile bool globalVar_running = false;

bool globalVar_getIsRunning();

void globalVar_reset();

void globalVar_start();

void globalVar_resetOld();

#endif /* GLOBALVAR_H_ */