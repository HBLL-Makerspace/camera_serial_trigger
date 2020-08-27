/*
 * displayDriver.c
 *
 * Created: 7/13/2020 4:08:30 PM
 *  Author: chadb
 */ 
#include "displayDriver.h"
#include "queue.h"
#include <stdbool.h>
#include <avr\interrupt.h>
#include <util\atomic.h>
#include "globalVar.h"
#include "eepromDriver.h"
#define COMMAND_QUEUE_SIZE 255
#define PAUSE_CYCLES 0
#define SS_PIN PORTB0
#define DISPLAY_CHAR_SIZE 80
#define LINE_LENGTH 20
#include <stdlib.h>

static queue_t* charQueue;
static bool pause;
static bool transmitting;
static char delayCycles;
static bool delay;
static unsigned char currentDisplayState[DISPLAY_CHAR_SIZE];
static unsigned char nextWritePos = 0;
static unsigned char oldRow, oldCol, row, col = 0;


static void addCommand(unsigned char command){
	queue_push(charQueue, command);
}

static void endTransmission(){
	PORTB |= 1 << SS_PIN;
	transmitting = false;
}

static void incrementWritePos(){
	nextWritePos++;
	col++;

	switch(nextWritePos){
		case 20:
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			displayDriver_setCurserPos(1, 0);
			row++;
			col = 0;
		}
		break;
		case 40:
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			displayDriver_setCurserPos(2, 0);
			row++;
			col = 0;
		}
		break;
		case 60:
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			displayDriver_setCurserPos(3, 0);
			row++;
			col = 0;
		}
		break;
		case 80:
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			displayDriver_setCurserPos(0, 0);
			row = col = 0;
		}
		break;
		default:
		break;
	}
}

static void decrementWritePos(){
	nextWritePos--;
	col--;

	switch(nextWritePos){
		case 19:
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			displayDriver_setCurserPos(0, 19);
			row = 0;
			col = 19;
		}
		break;
		case 39:
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			displayDriver_setCurserPos(1, 19);
			row = 1;
			col = 19;
		}
		break;
		case 59:
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			displayDriver_setCurserPos(2, 19);
			row = 2;
			col = 19;
		}
		break;
		case 0xff:
		nextWritePos = 79;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			displayDriver_setCurserPos(3, 19);
			row = 3;
			col = 19;
		}
		break;
		default:
		break;
	}
}

static void beginCommand(){
	addCommand(0xFE);
}

static void loadNextCommand(){
	unsigned char nextCommand = '\0';
	if(queue_elementCount(charQueue) > 0){
		nextCommand = queue_pop(charQueue);
		if(nextCommand == PAUSE_COMMAND){
			delayCycles = PAUSE_CYCLES;
			delay = true;
		} else {
			SPDR = nextCommand;
		}
	} else {
		endTransmission();
	}
}

static void restartTransmission(){
	delay = false;
	loadNextCommand();
}

void displayDriver_writeChar(unsigned char character){
	addCommand(character);
	currentDisplayState[nextWritePos] = character;
	incrementWritePos();
}

static void beginTransmission(){
	PORTB &= ~(1 << SS_PIN);
	delayCycles = PAUSE_CYCLES;
	delay = true;
	transmitting = true;
}





void displayDriver_spiInit(){
	DDRB |= 0x0f; //fixme
	PORTB |= 1 << SS_PIN;
	SPCR = 0b11011111;
}

void displayDriver_init(){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		charQueue = (queue_t *) malloc(sizeof(queue_t));
		displayDriver_spiInit();
		queue_init(charQueue, COMMAND_QUEUE_SIZE);
		pause = true;
		transmitting = false; 
		delayCycles = 0;
		delay = false;
		PORTB |= 1 << SS_PIN;
		displayDriver_clearDispaly();
	}
}	

void displayDriver_initCust(){
		cli();
		char displayInitString[30] = {0xFE, 0x54, 0x00, 0x00, 0x00, 0x00, 0x04, 0x0E, 0x1F, 0x00, 0x00, 0xFE, 0x54, 0x01,  0x00, 0x00, 0x1F, 0x0E, 0x04, 0x00, 0x00, 0x00, '/0'};
		for(uint8_t i = 0; i < 30; i++){
			addCommand(displayInitString[i]);
		}
		sei();
}


//run the tick function
void displayDriver_tick(){
	if(!pause){
		if(!transmitting){
			if(queue_elementCount(charQueue) > 0){
				beginTransmission();
			}
		} else {
			if(delay){
				if(delayCycles > 0){
					delayCycles--;
				} else {
					restartTransmission();
				}
			}
		}
	}
}

//pause driver
void displayDriver_pause(){
	pause = true;
}

//start driver
void displayDriver_start(){
	pause = false;
}

void displayDriver_addCommand(unsigned char command, bool pauseInterupts, bool delay){
	if(pauseInterupts){
		cli();
	}
	addCommand(command);
	if(delay){
		addCommand(PAUSE_COMMAND);
	}
	if(pauseInterupts){
		sei();
	}
}



void displayDriver_writeString(char* string){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		for(unsigned char i = 0; string[i] != '\0'; i++){
			unsigned myLetter = string[i];
			displayDriver_writeChar(myLetter);
		}
	}
}

void displayDriver_enableCurser(){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		addCommand(0xFE);
		addCommand(0x47);
		addCommand(PAUSE_COMMAND);
	}
}

void displayDriver_disableCurser(){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		addCommand(0xFE);
		addCommand(0x48);
		addCommand(PAUSE_COMMAND);
	}
}

void displayDriver_setCurserPos(unsigned char newRow, unsigned char newCol){
	unsigned char pos = 0;
	unsigned char charPos = 0;
	if(newCol >= LINE_LENGTH){
		newCol = 0;
	}
	switch(newRow){
		case 1:
			pos = 0x40;
			charPos = 20;
			break;
		case 2:
			pos = 0x14;
			charPos = 40;
			break;
		case 3:
			pos = 0x54;
			charPos = 60;
			break;
		case 0:
		default:
			pos = 0;
			charPos = 0;
			break;
	}
	pos += newCol;
	charPos += newCol;
	nextWritePos = charPos;
	row = newRow;
	col = newCol;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		beginCommand();
		addCommand(0x45);
		addCommand(pos);
		addCommand(PAUSE_COMMAND);
	}
}


void displayDriver_writeCustChar(unsigned char customNumber, unsigned char newRow, unsigned char newCol){
	displayDriver_setCurserPos(newRow, newCol);
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		addCommand(customNumber);
		addCommand(PAUSE_COMMAND);
		incrementWritePos();
	}
}

bool displayDriver_isTransmitting(){
	return transmitting;
}

ISR(SPI_STC_vect, ISR_BLOCK){
	loadNextCommand();
}

void displayDriver_clearDispaly(){
	for(unsigned char i = 0; i < DISPLAY_CHAR_SIZE; i++){
		currentDisplayState[i] = ' ';
	}
	nextWritePos = col = row = 0;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		beginCommand();
		addCommand(0x51);
		addCommand(PAUSE_COMMAND);
		displayDriver_curserHome();
	}
}

void displayDriver_displayOn(){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		beginCommand();
		addCommand(0x41);
		addCommand(PAUSE_COMMAND);
	}
}

void displayDriver_displayOff(){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		beginCommand();
		addCommand(0x42);
		addCommand(PAUSE_COMMAND);
	}
}

void displayDriver_curserHome(){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		nextWritePos = col = row = 0;
		beginCommand();
		addCommand(0x46);
		addCommand(PAUSE_COMMAND);
	}
}

void displayDriver_curserLeft(){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		beginCommand();
		addCommand(0x49);
	}
	decrementWritePos();
}

void displayDriver_curserRight(){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		beginCommand();
		addCommand(0x4A);
	}
	incrementWritePos();
}

void displayDriver_blinkCurserOn(){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		beginCommand();
		addCommand(0x4B);
		addCommand(PAUSE_COMMAND);
	}
}

void displayDriver_blinkCurserOff(){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		beginCommand();
		addCommand(0x4C);
		addCommand(PAUSE_COMMAND);
	}
}

void displayDriver_backspace(){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		beginCommand();
		addCommand(0x4E);
	}
	decrementWritePos();
}

void displayDriver_setContrast(char contrast){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		beginCommand();
		addCommand(0x52);
		addCommand(contrast);
		addCommand(PAUSE_COMMAND);
	}
}

void displayDriver_setBacklight(char brightness){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		beginCommand();
		addCommand(0x53);
		addCommand(brightness);
	}
}

void displayDriver_moveDispLeft(){
	
}

void displayDriver_moveDispRight(){
	
}

void displayDriver_writeDisplay(unsigned char * buffer){
	displayDriver_clearDispaly();
	for(int i = 0; i < 80; i++){
		displayDriver_writeChar(buffer[i]);
	}
}

unsigned char* displayDriver_getBuffer(){
	return currentDisplayState;
}

void displayDriver_loadBrightnessAndContrastFromEeprom(){
	unsigned char myBrightness = (unsigned char) eepromDriver_readDword(GLOBALVAR_BRIGHTNESS_MEM_SLOT);
	unsigned char myContrast = (unsigned char) eepromDriver_readDword(GLOBALVAR_CONTRAST_MEM_SLOT);
	displayDriver_setBacklight(myBrightness);
	displayDriver_setContrast(myContrast);
}

unsigned char displayDriver_getCurrentChar(){
	return currentDisplayState[nextWritePos];
}

void displayDriver_saveCursor(){
	oldRow = row;
	oldCol = col;
}


void displayDriver_restoreCursor(){
	displayDriver_setCurserPos(oldRow, oldCol);
}