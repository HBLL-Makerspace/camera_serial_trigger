/*
 * displayDriver.h
 *
 * Created: 7/13/2020 3:34:42 PM
 *  Author: chadb
 */ 


#ifndef DISPLAYDRIVER_H_
#define DISPLAYDRIVER_H_
#define PAUSE_COMMAND 0xA0
#include <stdbool.h>

void displayDriver_initCust();
void displayDriver_init();

//run the tick function
void displayDriver_tick();

//pause driver
void displayDriver_pause();

//start driver
void displayDriver_start();

void displayDriver_addCommand(unsigned char command, bool pauseInterupts, bool delay);

void displayDriver_writeString(char* string);

void displayDriver_enableCurser();

void displayDriver_disableCurser();

void displayDriver_setCurserPos(unsigned char newRow, unsigned char newCol);

void displayDriver_writeCustChar(unsigned char customNumber, unsigned char newRow, unsigned char newCol);

bool displayDriver_isTransmitting();

void displayDriver_clearDispaly();

void displayDriver_displayOn();

void displayDriver_displayOff();

void displayDriver_curserHome();

void displayDriver_curserLeft();

void displayDriver_curserRight();

void displayDriver_blinkCurserOn();

void displayDriver_blinkCurserOff();

void displayDriver_backspace();

void displayDriver_setContrast(char contrast);

void displayDriver_setBacklight(char brightness);

void displayDriver_moveDispLeft();

void displayDriver_moveDispRight();

void displayDriver_writeChar(unsigned char character);

void displayDriver_writeDisplay(unsigned char * buffer);

void displayDriver_loadBrightnessAndContrastFromEeprom();

unsigned char displayDriver_getCurrentChar();

unsigned char* displayDriver_getBuffer();

void displayDriver_saveCursor();

void displayDriver_restoreCursor();

#endif /* DISPLAYDRIVER_H_ */