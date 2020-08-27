/*
 * statusLight.c
 *
 * Created: 7/21/2020 3:11:44 PM
 *  Author: chadb
 */ 
#include "statusLight.h"
#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#define TICKS_PER_BIT 10
#define FINAL_DELAY 30

static bool normal = true;
unsigned char errorPattern;

static void statusLightOn(){
	PORTB |= 1 << PINB6;
}

static void statusLightOff(){
	PORTB &= ~(1 << PINB6);
}

static enum errorControl_st_t {
	init_st,
	begin_pattern_st,
	set_bit_st,
	wait_delay_st,
	final_delay_st
	} errorState;

void statusLight_init(){
	DDRB |= 1 << PINB6;
	PORTB |= 1 << PINB6;
	normal = true;
	errorState = init_st;
}


void statusLight_setNominal(){
	normal = true;
}

void statusLight_setError(unsigned char blinkPattern){
	errorPattern = blinkPattern;
	normal = false;
}

void statusLight_tick(){
	static char bitNumber = 0;
	static char bitMask = 0x80;
	static uint16_t counter = 0;
	switch (errorState) {
		case init_st:
			errorState = begin_pattern_st;
			break;
		case begin_pattern_st:
			if(normal){
				errorState = begin_pattern_st;
				statusLightOn();
			}else{
				errorState = set_bit_st;
				bitNumber = 0;
				counter = 0;
				bitMask = 0x80;
			}
			break;
		case set_bit_st:
			if(bitNumber < 8){
				if(errorPattern & bitMask){
					statusLightOn();
				} else {
					statusLightOff();
				}
				bitMask = bitMask >> 1;
				bitNumber++;
				errorState = wait_delay_st;
			} else {
				errorState = final_delay_st;
				statusLightOff();
			}
			break;
		case wait_delay_st:
			if(counter > TICKS_PER_BIT){
				errorState = set_bit_st;
			} else {
				errorState = wait_delay_st;
			}
			break;
		case final_delay_st:
			if(counter > FINAL_DELAY){
				errorState = begin_pattern_st;
			} else {
				errorState = final_delay_st;
			}
			break;
		default:
			break;
	}
	
	switch (errorState){
		case init_st:
		case begin_pattern_st:
		case set_bit_st:
			counter = 0;
			break;
		case wait_delay_st:
		case final_delay_st:
			counter++;
			break;
		default:
			break;
	}
}