/*
 * menuDriver.h
 *
 * Created: 8/11/2020 1:14:49 PM
 *  Author: chadb
 */ 


#ifndef MENUDRIVER_H_
#define MENUDRIVER_H_
#include "globalVar.h"
#include <stdint.h>

void menuDriver_init();
void menuDriver_configure(uint8_t numItems, handler backCommand);
void menuDriver_addItem(char* itemName, handler newHandler, uint8_t handlerNumber);
void menuDriver_drawMenu();
void menuDriver_tick();
uint8_t menuDriver_getLastMenuIndex();
void menuDriver_resetIndex();
void menuDriver_setIndex(uint8_t index);

#endif /* MENUDRIVER_H_ */