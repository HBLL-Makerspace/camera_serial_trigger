/*
 * buttonDriver.c
 *
 * Created: 7/21/2020 4:56:20 PM
 *  Author: chadb
 */ 
#include "buttonDriver.h"
#include <stdbool.h>
#include <avr/io.h>
#include <stdint.h>

#define PORT_DEF PORTL
#define DD_DEF DDRL
#define PIN_DEF PINL
#define ZERO_DELIM 1 //put 1 if not, 0 if is
#define CHANNEL_WIDTH 8

#define VC_DEC_OR_SET(high, low, mask) \
low = ~(low & mask); \
high = low ^ (high & mask)
static uint8_t button_state = 0;

static handler handlerArray[16];

void voidFunc(){
	//do nothing
}

void buttonDriver_clearAllHandlers(){
	for(unsigned char i = 0; i < (CHANNEL_WIDTH * 2); i++){
		handlerArray[i] = voidFunc;
	}
}

unsigned char getHandlerNumber(unsigned char handlerChannel, bool turnOnHandler){
	if(turnOnHandler){
		return handlerChannel - ZERO_DELIM;
		} else {
		return ((handlerChannel - ZERO_DELIM) + CHANNEL_WIDTH);
	}
}

void clearHandler(unsigned char handlerChannel, bool turnOnHandler){
	handlerArray[getHandlerNumber(handlerChannel, turnOnHandler)] = voidFunc;
}



void buttonDriver_init(){
	PORT_DEF = 0xff;
	DDRL = 0xff;
	buttonDriver_clearAllHandlers();
}

void buttonDriver_runButtonFunctions(){
	static uint8_t lastButtonState = 0;
	uint8_t changedState = lastButtonState ^ button_state;
	uint8_t mask = 1; 
	for(uint8_t i = 0; i < CHANNEL_WIDTH; i++){
		if(changedState & mask){
			if(button_state & mask){
				handlerArray[i]();
			} else {
				handlerArray[i + CHANNEL_WIDTH]();
			}
		}
		mask = mask << 1;
	}
	lastButtonState = button_state;
}

void buttonDriver_tick(){
	// Eight vertical two bit counters for number of equal states
	static uint8_t vcount_low = 0xFF, vcount_high = 0xFF;
	// Keeps track of current (debounced) state

	// Read buttons (active low so invert with ~). Xor with
	// button_state to see which ones are about to change state
	uint8_t state_changed = ~PIN_DEF ^ button_state;
	// Decrease counters where state_changed = 1, set the others to 0b11.
	VC_DEC_OR_SET(vcount_high, vcount_low, state_changed);
	// Update state_changed to have a 1 only if the counter overflowed
	state_changed &= vcount_low & vcount_high;
	// Change button_state for the buttons who’s counters rolled over
	button_state ^= state_changed;
	// Update button_down with buttons who’s counters rolled over
	// and who’s state is 1 (pressed)
}

void buttonDriver_addHandler(char handlerChannel, bool turnOnHandler, handler newHandler){
	 handlerArray[getHandlerNumber(handlerChannel, turnOnHandler)] = newHandler;
}

unsigned char buttonHandler_getPortStatus(){
	return PIN_DEF;
}
 
unsigned char buttonDriver_getDebouncedPortStatus(){
	return button_state;
}

