/*
 * globalVar.c
 *
 * Created: 7/29/2020 4:34:49 PM
 *  Author: chadb
 */ 
#include "globalVar.h"
#include "avr/io.h"
#include "eepromDriver.h"
#include "avr/interrupt.h"
#include <stdbool.h>

bool globalVar_getIsRunning(){
	return globalVar_running;
}

void globalVar_reset(){
	eepromDriver_writeDword(GLOBALVAR_RESET_MEM_SLOT, true);
	cli();
	while(1);
}

void globalVar_start(){
	globalVar_running = true;
}

void globalVar_resetOld(){
	globalVar_running = false;
}