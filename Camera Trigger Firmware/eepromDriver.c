/*
 * eepromDriver.c
 *
 * Created: 7/30/2020 2:59:07 PM
 *  Author: chadb
 */ 

#include <avr\eeprom.h>
#include <util\atomic.h>

#define SLOT_WIDTH 4


void eepromDriver_writeDword(uint8_t index, uint32_t value){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		uint32_t* address = (uint32_t*) (index * SLOT_WIDTH);
		eeprom_update_dword(address, value);
	}
}

void eepromDriver_writeFloat(uint8_t index, float value){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		float* address = (float*) (index * SLOT_WIDTH);
		eeprom_update_float(address, value);
	}
}

uint32_t eepromDriver_readDword(uint8_t index){
	uint32_t dword;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		uint32_t* address = (uint32_t*) (index * SLOT_WIDTH);
		dword = eeprom_read_dword(address);
	}
	return dword;
}

float eepromDriver_readFloat(uint8_t index){
	float myFloat;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		float* address = (float*) (index * SLOT_WIDTH);
		myFloat = eeprom_read_float(address);
	}
	return myFloat;
}