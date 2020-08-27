/*
 * statusDriverLine.c
 *
 * Created: 7/31/2020 11:10:17 AM
 *  Author: chadb
 */ 
#include <avr\io.h>
#include <stdbool.h>
#include "displayDriver.h"

static bool dispActive = false;
static bool showStatus = false;
static bool redraw = false;

void statusDriverLine_init(){
	DDRE &= ~(1 << PINE4);
	DDRE |= (1 << PINE5);
	PORTE &= ~(1 << PINE4);
	dispActive = false;
	showStatus = true;
	statusDriverLine_setStatus(true);
}

static void setActive(){
	displayDriver_setCurserPos(0, 4);
	displayDriver_writeString("ACTIVE");
	displayDriver_setCurserPos(1, 0);
	dispActive = true;
}

static void setInactive(){
	displayDriver_setCurserPos(0, 4);
	displayDriver_writeString("IDLE  ");
	displayDriver_setCurserPos(1, 0);
	dispActive = false;
}

void statusDriverLine_setStatus(bool enable){
	showStatus = enable;
	if(enable){
		redraw = true;
	}
}

void statusDriverLine_tick(){
	if(showStatus){
		if(redraw){
			displayDriver_setCurserPos(0, 0);
			displayDriver_writeString("USB:");
			setInactive();
		
			redraw = false;
		}
		if(PINE & (1 << PINE4)){
			if(!dispActive){
				setActive();
			}
		} else {
			if(dispActive){
				setInactive();
			}
		}
	}
}