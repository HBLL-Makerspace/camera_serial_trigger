/*
 * buttonDriver.h
 *
 * Created: 7/21/2020 4:56:07 PM
 *  Author: chadb
 */ 

#include <stdbool.h>
#include "globalVar.h"


#ifndef BUTTONDRIVER_H_
#define BUTTONDRIVER_H_

void voidFunc();
void buttonDriver_init();
void buttonDriver_tick();
void buttonDriver_addHandler(char handlerChannel, bool turnOnHandler, handler newHandler);
unsigned char buttonHandler_getPortStatus();
unsigned char buttonDriver_getDebouncedPortStatus();
void buttonDriver_clearAllHandlers();
void buttonDriver_runButtonFunctions();



#endif /* BUTTONDRIVER_H_ */