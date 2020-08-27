/*
 * eepromDriver.h
 *
 * Created: 7/30/2020 2:58:40 PM
 *  Author: chadb
 */ 


#ifndef EEPROMDRIVER_H_
#define EEPROMDRIVER_H_
#include <stdint.h>


void eepromDriver_writeDword(uint8_t index, uint32_t value);
void eepromDriver_writeFloat(uint8_t index, float value);
uint32_t eepromDriver_readDword(uint8_t index);
float eepromDriver_readFloat(uint8_t index);


#endif /* EEPROMDRIVER_H_ */