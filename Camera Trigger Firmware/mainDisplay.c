/*
 * mainDisplay.c
 *
 * Created: 8/11/2020 3:58:26 PM
 *  Author: chadb
 */ 
#include "mainDisplay.h"
#include "displayDriver.h"
#include "buttonDriver.h"
#include "shutterDriver.h"
#include "menuDriver.h"
#include "globalVar.h"
#include "numberEditor.h"
#include "statusLight.h"
#include "eepromDriver.h"
#include "eventEditor.h"
#include "timingEngine.h"
#include <avr/interrupt.h>
#include <stdint.h>
#include <avr/wdt.h>
#define CONTRAST_MIN 1
#define CONTRAST_MAX 50
#define BRIGHTNESS_MIN 1
#define BRIGHTNESS_MAX 8
#define SET_COUNT_MIN 2
#define SET_COUNT_MAX 16
static uint8_t oldMenuIndex = 0;

static void resetFactory(){
	cli();
	for(uint8_t i = 0; i < GLOBALVAR_MAX_EVENTS; i++){
		eepromDriver_writeDword(GLOBALVAR_TIMING_BEGIN_MEM_SLOT + i, 0);
		wdt_reset();
	}
	eepromDriver_writeDword(GLOBALVAR_BRIGHTNESS_MEM_SLOT, 4);
	wdt_reset();
	eepromDriver_writeDword(GLOBALVAR_CONTRAST_MEM_SLOT, 32);
	wdt_reset();
	eepromDriver_writeDword(GLOBALVAR_RESET_MEM_SLOT, 0);
	eepromDriver_writeDword(GLOBALVAR_TIMING_SETPOINTS, 0);
	wdt_reset();
	eepromDriver_writeDword(GLOBALVAR_TIMING_SET_COUNT, 0);
	sei();
	statusLight_setError(0b11001101);
	displayDriver_clearDispaly();
	displayDriver_writeString("Please Cycle Power \0");
	displayDriver_setCurserPos(1, 0);
	displayDriver_writeString("Device Reset Successful");
}

void mainDisplay_reopenMenu(){
	menuDriver_setIndex(oldMenuIndex);
	mainDisplay_openMenu();
}

static void initEditor(){
	eventEditor_loadEeprom();
	oldMenuIndex = menuDriver_getLastMenuIndex();
	menuDriver_resetIndex();
	eventEditor_init();
}

static void handleContrast(uint8_t value){
	eepromDriver_writeDword(GLOBALVAR_CONTRAST_MEM_SLOT, value);
	displayDriver_loadBrightnessAndContrastFromEeprom();
}

static void editContrast(){
	uint8_t oldValue = (uint8_t) eepromDriver_readDword(GLOBALVAR_CONTRAST_MEM_SLOT);
	numberEditor_initInt(oldValue, handleContrast, mainDisplay_openMenu, "Contrast Setting\0", "Value", CONTRAST_MIN, CONTRAST_MAX);
}

static void handleBrightness(uint8_t value){
	eepromDriver_writeDword(GLOBALVAR_BRIGHTNESS_MEM_SLOT, value);
	displayDriver_loadBrightnessAndContrastFromEeprom();
}

static void editBrightness(){
	uint8_t oldValue = (uint8_t) eepromDriver_readDword(GLOBALVAR_BRIGHTNESS_MEM_SLOT);
	numberEditor_initInt(oldValue, handleBrightness, mainDisplay_openMenu, "Brightness Setting\0", "Value", BRIGHTNESS_MIN, BRIGHTNESS_MAX);
}


static void handleEventCount(uint8_t value){
	eepromDriver_writeDword(GLOBALVAR_TIMING_SET_COUNT, value);
	mainDisplay_openMenu();
}

static void editEventCount(){
	uint8_t oldValue = (uint8_t) eepromDriver_readDword(GLOBALVAR_TIMING_SET_COUNT);
	numberEditor_initInt(oldValue, handleEventCount, mainDisplay_openMenu, "Trigger Event Count\0", "Triggers", SET_COUNT_MIN, SET_COUNT_MAX);
}

void mainDisplay_openMenu(){
	displayDriver_clearDispaly();
	statusDriverLine_init();
	menuDriver_configure(5, mainDisplay_init);
	menuDriver_addItem("Set Contrast       \0", editContrast, 3);	
	menuDriver_addItem("Set Brightness     \0", editBrightness, 2);
	menuDriver_addItem("Set Event Count    \0", editEventCount, 0);
	menuDriver_addItem("Edit Events        \0", initEditor, 1);
	menuDriver_addItem("Factory Reset      \0", resetFactory, 4);
	menuDriver_drawMenu();
}

void mainDisplay_init(){
	
	buttonDriver_clearAllHandlers();
	buttonDriver_addHandler(7, true, timingEngine_loadAndRunEvents);

//	buttonDriver_addHandler(7, true, shutterDriver_enableAllShutter);
//	buttonDriver_addHandler(7, false, shutterDriver_disableAllShutter);
	buttonDriver_addHandler(8, true, shutterDriver_toggleFocus);
	buttonDriver_addHandler(4, true, mainDisplay_openMenu);
	displayDriver_clearDispaly();
	statusDriverLine_init();
	displayDriver_setCurserPos(1, 7);
	displayDriver_writeString("Ready\0");
	displayDriver_setCurserPos(3, 0);
	displayDriver_writeString("Menu:Sel\0");
	menuDriver_resetIndex();
}