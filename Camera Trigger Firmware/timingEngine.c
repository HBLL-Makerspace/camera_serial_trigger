/*
 * timingEngine.c
 *
 * Created: 7/29/2020 4:43:45 PM
 *  Author: chadb
 */ 
#include "timingEngine.h"
#include "eepromDriver.h"
#include "globalVar.h"
#include "shutterDriver.h"
#include <stdint.h>
static uint16_t timings[GLOBALVAR_MAX_EVENTS];
static uint8_t eventCount;
static uint32_t eventTypes;

static enum timeEngine_t {
	init_st,
	load_next_event_st,
	delay_st,
	set_event_st,
	done_st
} engineState;

void timingEngine_init(){
	engineState = done_st;
}

static void setEvent(uint8_t event){
	switch (event){
		case GLOBALVAR_EN_SHUT:
			shutterDriver_enableAllShutter();
			break;
		case GLOBALVAR_EN_FOC:
			shutterDriver_enableAllFocus();
			break;
		case GLOBALVAR_DS_FOC:
			shutterDriver_disableAllFocus();
			break;
		case GLOBALVAR_DS_SHUT:
			shutterDriver_disableAllShutter();
			break;
		default:
			break;
	}
}

static uint8_t getNextEvent(){
	uint8_t temp = (uint8_t)(eventTypes & 0b11);
	eventTypes = (eventTypes >> 2);
	return temp;
}
void timingEngine_tick(){
	static uint8_t currentEventCount = 0;
	static uint16_t counter = 0;
	static uint8_t nextEvent = 0;
	switch(engineState){
		case init_st:
			engineState = load_next_event_st;
			currentEventCount = 0;
			break;
		case load_next_event_st:
			if(counter <= 1){
				engineState = set_event_st;
			} else {
				counter --;
				engineState = delay_st;
			}
			break;
		case delay_st:
			if(counter <= 1){
				engineState = set_event_st;
			} else {
				engineState = delay_st;
			}
			break;
		case set_event_st:
			if(currentEventCount >= eventCount){
				engineState = done_st;
			} else {
				engineState = load_next_event_st;
			}
			break;
		case done_st:
			engineState = done_st;
			break;
		default:
			engineState = init_st;
			break;
	}
	switch (engineState){
		case init_st:
			currentEventCount = 0;
			break;
		case load_next_event_st:
			counter = timings[currentEventCount];
			nextEvent = getNextEvent();
			currentEventCount ++;
			break;
		case delay_st:
			counter --;
			break;
		case set_event_st:
			setEvent(nextEvent);
			break;
		case done_st:
			break;
		default:
			statusLight_setError(0b10101101);
	}
}
void timingEngine_loadTimings(){
	eventTypes = eepromDriver_readDword(GLOBALVAR_TIMING_SETPOINTS);
	eventCount = eepromDriver_readDword(GLOBALVAR_TIMING_SET_COUNT);
	for (uint8_t i = 0; i < GLOBALVAR_MAX_EVENTS; i++){
		timings[i] = (uint16_t)(eepromDriver_readFloat(GLOBALVAR_TIMING_BEGIN_MEM_SLOT + i) / TIMINGENGINE_TICK_TIME);
	}
	engineState = init_st;
}