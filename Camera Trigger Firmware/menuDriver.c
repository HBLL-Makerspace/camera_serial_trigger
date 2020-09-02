/*
 * menuDriver.c
 *
 * Created: 8/11/2020 1:15:18 PM
 *  Author: chadb
 */ 
#include "displayDriver.h"
#include "buttonDriver.h"
#include "globalVar.h"
#include <stdint.h>
#define MAX_MENU_ITEMS 40
handler handlers[MAX_MENU_ITEMS];
handler back;
uint8_t itemCount = 0;
char* handlerMenuText[MAX_MENU_ITEMS];
#define BLANKLINE "                    "
#define SHORT_BLANK_LINE "                   "
uint8_t itemSelected = 0;
static bool draw = false;


void menuDriver_init(){
	for(uint8_t i = 0; i < MAX_MENU_ITEMS; i++){
		handlerMenuText[i] = (char*) malloc(20 * sizeof(unsigned char));
		handlerMenuText[i][19] = '\0';
	}
}

void menuDriver_configure(uint8_t numItems, handler backCommand){
	itemCount = numItems;
	back = backCommand;
}

void menuDriver_addItem(char* itemName, handler newHandler, uint8_t handlerNumber){
	handlers[handlerNumber] = newHandler;
	for(uint8_t i = 0; i < 19; i++){
		handlerMenuText[handlerNumber][i] = itemName[i];
	}
	handlerNumber++;
}

static void drawMenu(){
	displayDriver_setCurserPos(2, 1);
	displayDriver_writeString(handlerMenuText[itemSelected]);
	displayDriver_setCurserPos(1, 1);
	if(itemSelected > 0){
		displayDriver_writeString(handlerMenuText[itemSelected - 1]);
	} else {
		displayDriver_writeString(SHORT_BLANK_LINE);
	}
	displayDriver_setCurserPos(3, 1);
	if(itemSelected < itemCount - 1){
		displayDriver_writeString(handlerMenuText[itemSelected + 1]);
	} else {
		displayDriver_writeString(SHORT_BLANK_LINE);
	}
}

static void down(){
	if(itemSelected < (itemCount - 1)){
		itemSelected++;
		drawMenu();
	}
}

static void up(){
	if(itemSelected != 0){
		itemSelected--;
		drawMenu();
	}
}

static void select(){
	handlers[itemSelected]();
}

void menuDriver_drawMenu(){
	for(uint8_t i = 1; i < 4; i++){
		displayDriver_setCurserPos(i, 0);
		displayDriver_writeString(BLANKLINE);
	}
	draw = true;
	usart0_send("drawing menu\r\n");
}

void menuDriver_tick(){
	if(draw){
		drawMenu();
		displayDriver_setCurserPos(2, 0);
		displayDriver_writeChar(0x7E);
		buttonDriver_clearAllHandlers();
		buttonDriver_addHandler(5, true, down);
		buttonDriver_addHandler(3, true, up);
		buttonDriver_addHandler(4, true, select);
		buttonDriver_addHandler(1, true, back);
		draw = false;
	}
}

uint8_t menuDriver_getLastMenuIndex(){
	return itemSelected;
}

void menuDriver_resetIndex(){
	itemSelected = 0;
}

void menuDriver_setIndex(uint8_t index){
	itemSelected = index;
}
