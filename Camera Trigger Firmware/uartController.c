// /*
//  * uartController.c
//  *
//  * Created: 7/23/2020 2:50:25 PM
//  *  Author: chadb
//  */

// #include <avr/io.h>
// #include <avr/interrupt.h>
// #include <util/atomic.h>
// #include <stdbool.h>
// #include <avr/eeprom.h>
// #include "uartController.h"
// #include "displayDriver.h"
// #include "globalVar.h"
// #include "statusLight.h"
// #include "eepromDriver.h"

// #define BAUD_REG 103
// #define DOUBLE_FREQ 1

// static bool uartError;
// static void recieveInteruptEnable(){
// 	UCSR0B |= 1 << RXCIE0;

// 	UCSR0B |= 1 << RXEN0;

// }

// void uartConteroller_tick(){
// 	static bool lastErrorState = false;
// 	if(!lastErrorState && uartError){
// 		displayDriver_clearDispaly();
// 		displayDriver_writeString("Frame Error");
// 	}
// 	lastErrorState = uartError;
// }

// void uartController_init(){
// 	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
// 		UBRR0 = BAUD_REG;
// 		UCSR0A |= DOUBLE_FREQ <<  U2X0;
// 		recieveInteruptEnable();
// 		DDRE &= ~(1 << PINE0);
// 	}
// }

// ISR(USART0_RX_vect, ISR_BLOCK){
// 	char myChar = UDR0;
// 	uint32_t dword = 0;
// 	static bool nextIndex = false;
// 	static bool nextValue = false;
// 	static int index = 0;
// 	if(UCSR0A & (1 << FE0)){
// 		if(!uartError){
// 			statusLight_setError(0b01001001);
// 			displayDriver_clearDispaly();
// 			displayDriver_writeString("Frame Error");
// 			uartError = true;
// 			myChar = UDR0;
// 		}
// 	} else if(nextIndex) {
// 		index = myChar;
// 		nextValue = true;
// 		nextIndex = false;
// 	} else if(nextValue) {
// 		dword = myChar;
// 		eepromDriver_writeDword(index, dword);
// 		nextValue = false;
// 	} else {
// 		if(myChar == 0x80){
// 			globalVar_reset();
// 		} else if(myChar == 0x81){
// 			displayDriver_clearDispaly();
// 			displayDriver_writeString("Take Picture Now");
// 		} else if(myChar == 0x82){
// 			nextIndex = true;
// 		} else if( myChar == 0x7f){
// 			displayDriver_backspace();
// 		} else {
// 			displayDriver_writeChar(myChar);
// 		}
// 	}
// }