/*
 * shutterDriver.c
 *
 * Created: 7/29/2020 1:17:54 PM
 *  Author: chadb
 */ 
#include "shutterDriver.h"
#include <avr/io.h>

#define  SHUT_A_PIN 1
#define  SHUT_B_PIN 3
#define	 FOCUS_A_PIN 0
#define  FOCUS_B_PIN 2
#define  FOCUS_BM 0b00000101
#define  SHUT_BM 0b00001010

void shutterDriver_init(){
	DDRK = 0x0f;
	PORTK = 0x00;
}

void shutterDriver_toggleFocus(){
	PINK = FOCUS_BM;
}

void shutterDriver_enableChanAFocus(){
	PORTK |= (1 << FOCUS_A_PIN);
}

void shutterDriver_enableChanBFocus(){
	PORTK |= (1 << FOCUS_B_PIN);
}

void shutterDriver_enableChanAShutter(){
	PORTK |= (1 << SHUT_A_PIN);
}

void shutterDriver_enableChanBShutter(){
	PORTK |= (1 << SHUT_B_PIN);
}

void shutterDriver_disableChanAFocus(){
	PORTK &= ~(1 << FOCUS_A_PIN);
}

void shutterDriver_disableChanBFocus(){
	PORTK &= ~(1 << FOCUS_B_PIN);
}

void shutterDriver_disableChanAShutter(){
	PORTK &= ~(1 << SHUT_A_PIN);
}

void shutterDriver_disableChanBShutter(){
	PORTK &= ~(1 << SHUT_B_PIN);
}

void shutterDriver_enableAllFocus(){
	PORTK |= FOCUS_BM;
}
void shutterDriver_enableAllShutter(){
	PORTK |= SHUT_BM;
}
void shutterDriver_disableAllFocus(){
	PORTK &= ~FOCUS_BM;
}
void shutterDriver_disableAllShutter(){
	PORTK &= ~SHUT_BM;
}