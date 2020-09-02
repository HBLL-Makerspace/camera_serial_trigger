/*
 * Camera Trigger Firmware.c
 *
 * Created: 7/9/2020 1:44:28 PM
 * Author : chadb
 */

#include "displayDriver.h"
#include "eepromDriver.h"
#include "events.h"
#include "menuDriver.h"
#include "numberEditor.h"
#include "statusDriverLine.h"
#include "statusLight.h"
#include "timingEngine.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#define F_CPU 8000000UL
#include <util/delay.h>

#include "buttonDriver.h"
#include "uartController.h"

#include "globalVar.h"
#include "shutterDriver.h"

#define TIMER_3_PRESCALER 0b011
#define TIMER_3_COMPARE_H 0x09
#define TIMER_3_COMPARE_L 0x8E
#define TIMER_CTC_MODE_H 0b00001000 // mask with |

unsigned char buffer[80];
unsigned char brightness = 1;

void enableBrownout() {
    MCUSR |= 1 << BORF;
    MCUSR |= 1 << PORF;
}
void voidAFunc(uint8_t value) {}

void back() { displayDriver_curserLeft(); }

void backspace() { displayDriver_backspace(); }

void wakePC() {
    static int i = 0;
    i++;
}

void forward() { displayDriver_curserRight(); }

void up() {
    cli();
    while (1)
        ;
}

void down() { PORTE &= ~(1 << PINE5); }

void release() { PORTE |= (1 << PINE5); }

void read() { globalVar_reset(); }

void brightnessAdjust() {
    statusLight_setNominal();
    brightness++;
    if (brightness > 8) {
        brightness = 2;
    }
    displayDriver_setBacklight(brightness);
}

void extra() {
    statusLight_setError(0b11100101);
    brightness--;
    if (brightness < 1) {
        brightness = 1;
    }
}

void watchDogTimer() {
    cli();

    /* Start timed sequence */
    WDTCSR |= (1 << WDCE) | (1 << WDE);
    /* Set new prescaler(time-out) value = 64K cycles (~0.5 s) */
    WDTCSR = (1 << WDE) | (1 << WDP2) | (1 << WDP0);
    sei();
}

void setUpSystemTimer() {
    TCCR3B |= (TIMER_3_PRESCALER | TIMER_CTC_MODE_H);
    OCR3AH = TIMER_3_COMPARE_H;
    OCR3AL = TIMER_3_COMPARE_L;
    TIMSK3 |= 1 << OCIE3A;
}

void status() {
    displayDriver_clearDispaly();
    statusDriverLine_init();
}

int main(void) {
    usart0_init();
    usart0_send("hello\r\n");
	//events_resetEvents();
    events_loadEventsFromEEPROM();

    wdt_enable(3);
    enableBrownout();
    cli();
    globalVar_start();
    DDRE |= 1 << PINE4;
    PORTE |= 1 << PINE4;
    DDRL = 0xff;
    PORTL = 0xff;
    menuDriver_init();
    shutterDriver_init();
    // uartController_init();
    buttonDriver_init();
    // buttonDriver_addHandler(1, true, test);
    buttonDriver_addHandler(2, true, read);
    buttonDriver_addHandler(4, true, down);
    buttonDriver_addHandler(4, false, menuDriver_drawMenu);
    buttonDriver_addHandler(3, true, shutterDriver_enableAllFocus);
    buttonDriver_addHandler(5, true, shutterDriver_disableAllFocus);
    buttonDriver_addHandler(6, true, back);
    //	buttonDriver_addHandler(7, true, shutterDriver_enableAllShutter);
    //	buttonDriver_addHandler(7, false, shutterDriver_disableAllShutter);
    buttonDriver_addHandler(8, true, shutterDriver_toggleFocus);
    //		buttonDriver_clearAllHandlers();

    /* Replace with your application code */

    displayDriver_init();
    displayDriver_loadBrightnessAndContrastFromEeprom();
    displayDriver_start();

    //	displayDriver_enableCurser();
    if (MCUSR & (1 << WDRF)) {
        bool wasReset = eepromDriver_readDword(GLOBALVAR_RESET_MEM_SLOT);
        if (!wasReset) {
            statusLight_setError(0b10011001);
        } else {
            eepromDriver_writeDword(GLOBALVAR_RESET_MEM_SLOT, false);
        }
        MCUSR &= ~(1 << WDRF);
    } else {
        statusLight_setNominal();
    }
    for (uint8_t i = 0; i < 100; i++) {
        wdt_reset();
        displayDriver_tick();
        _delay_ms(10);
    }
    displayDriver_initCust();
    for (uint8_t i = 0; i < 100; i++) {
        wdt_reset();
        displayDriver_tick();
        _delay_ms(10);
    }
    statusDriverLine_init();
    for (uint8_t i = 0; i < 100; i++) {
        wdt_reset();
        displayDriver_tick();
        _delay_ms(10);
    }
    cli();
    statusLight_init();
    //	setUpSystemTimer();
    mainDisplay_init();
    timingEngine_init();

    sei();
    //	while(1);
    while (1) {
        _delay_ms(10);
        wdt_reset();
        statusDriverLine_tick();
        statusLight_tick();
        // uartConteroller_tick();
        buttonDriver_tick();
        menuDriver_tick();
        buttonDriver_runButtonFunctions();
        displayDriver_tick();
        timingEngine_tick();
        wdt_reset();
    }
}

// ISR(TIMER3_COMPA_vect, ISR_NOBLOCK){
// wdt_reset();
// statusDriverLine_tick();
// statusLight_tick();
// uartConteroller_tick();
// buttonDriver_tick();
// menuDriver_tick();
// buttonDriver_runButtonFunctions();
// displayDriver_tick();
//}