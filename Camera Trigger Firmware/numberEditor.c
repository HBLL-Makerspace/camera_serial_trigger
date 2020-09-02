/*
 * numberEditor.c
 *
 * Created: 8/6/2020 3:30:14 PM
 *  Author: chadb
 */

#include "buttonDriver.h"
#include "displayDriver.h"
#include "events.h"
#include "globalVar.h"
#include "printf.h"
#include "statusDriverLine.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define START_COL 3
#define MAX_EDIT_COL 4
#define MAX_EDIT_COL_INT 3
static bool floatFunc = false;
static void (*returnIntFunction)(uint32_t);
static void (*returnFloatFunction)(Event event);
static float myFloat = 0.0;
static uint32_t myInt = 0;
static float stepValue = 0.0;
static float minValue = 0.0;
static float maxValue = 0.0;
static char title[21];
static uint8_t currentCollumn = 0;
static uint8_t col = 0;
static uint8_t eventType;

static void copyTitle(char *string) {
    for (uint8_t i = 0; i < 21; i++) {
        title[i] = string[i];
    }
}

static void setEditColumn(uint8_t newColumn) {
    displayDriver_setCurserPos(1, currentCollumn);
    displayDriver_writeChar(' ');
    displayDriver_setCurserPos(3, currentCollumn);
    displayDriver_writeChar(' ');
    displayDriver_writeCustChar(0, 1, newColumn);
    displayDriver_writeCustChar(1, 3, newColumn);
    displayDriver_setCurserPos(2, newColumn);
    currentCollumn = newColumn;
}

static void incrementDisplay() {
    bool doubleBack = false;
    unsigned char newChar = (displayDriver_getCurrentChar() + 1);
    if (newChar == '/') {
        displayDriver_curserLeft();
        newChar = displayDriver_getCurrentChar() + 1;
        doubleBack = true;
    }
    if (newChar > '9') {
        displayDriver_curserLeft();
        incrementDisplay();
        displayDriver_curserRight();
        newChar = '0';
    }
    displayDriver_writeChar(newChar);
    displayDriver_curserLeft();
    if (doubleBack) {
        displayDriver_curserRight();
    }
}

static void decrementDisplay() {
    bool doubleBack = false;
    unsigned char newChar = (displayDriver_getCurrentChar() - 1);
    if (newChar == '-') {
        displayDriver_curserLeft();
        newChar = displayDriver_getCurrentChar() - 1;
        doubleBack = true;
    }
    if (newChar < '0') {
        displayDriver_curserLeft();
        decrementDisplay();
        displayDriver_curserRight();
        newChar = '9';
    }
    displayDriver_writeChar(newChar);
    displayDriver_curserLeft();
    if (doubleBack) {
        displayDriver_curserRight();
    }
}

static void right() {
    uint8_t maxCol;
    if (floatFunc) {
        maxCol = MAX_EDIT_COL;
    } else {
        maxCol = MAX_EDIT_COL_INT;
    }
    if (col < (maxCol)) {
        col++;
        stepValue = stepValue / 10;
        setEditColumn(col + START_COL);
        if (displayDriver_getCurrentChar() == '.') {
            col++;
            setEditColumn(col + START_COL);
        }
    }
}

static void left() {
    if (col > 0) {
        col--;
        stepValue = stepValue * 10;
        setEditColumn(col + START_COL);
        if (displayDriver_getCurrentChar() == '.') {
            col--;
            setEditColumn(col + START_COL);
        }
    }
}

static void increment() {
    if (!floatFunc) {
        if ((myInt + stepValue) <= maxValue && !(myInt + stepValue < myInt)) {
            incrementDisplay();
            myInt += stepValue;
            return;
        }
    }
    float temp = myFloat + stepValue;

    if (!floatFunc) {
        temp = myInt + stepValue;
    }
    if (temp <= maxValue) {
        incrementDisplay();
        if (floatFunc) {
            myFloat = temp;
        } else {
            myInt++;
        }
    }
}

static void decrement() {
    if (!floatFunc) {
        if ((myInt - stepValue) >= minValue && (myInt - stepValue) <= myInt) {
            decrementDisplay();
            myInt -= stepValue;
            return;
        }
    }
    float temp = myFloat - stepValue;
    if (!floatFunc) {
        temp = myInt - stepValue;
    }
    if (temp >= minValue) {
        decrementDisplay();
        if (floatFunc) {
            myFloat = temp;
        } else {
            myInt--;
        }
    }
}

void intReturnCall() {
    uint32_t temp = myInt;
    returnIntFunction(temp);
}

void floatReturnCall() {
    Event event;
    event.type = eventType;
    event.data.value = (uint32_t)(myFloat * 100);
    returnFloatFunction(event);
}

void numberEditor_initFloat(float oldValue, void (*returnFunction)(float), handler backFunction,
                            char *valueName, char *valueLabel, float min, float max,
                            uint8_t eventType) {
    statusDriverLine_setStatus(false);
    eventType = eventType;
    floatFunc = true;
    col = 4;
    myFloat = oldValue;
    returnFloatFunction = returnFunction;
    displayDriver_disableCurser();
    copyTitle(valueName);
    minValue = min;
    maxValue = max;
    displayDriver_clearDispaly();
    displayDriver_tick();
    displayDriver_writeString(title);
    char myString[7];
    sprintf(myString, "%05.2f", myFloat);
    displayDriver_setCurserPos(2, START_COL);
    displayDriver_writeString(myString);
    displayDriver_writeChar(' ');
    displayDriver_writeString(valueLabel);
    setEditColumn(START_COL + MAX_EDIT_COL);
    stepValue = 0.01;
    buttonDriver_clearAllHandlers();
    buttonDriver_addHandler(3, true, increment);
    buttonDriver_addHandler(5, true, decrement);
    buttonDriver_addHandler(2, true, right);
    buttonDriver_addHandler(6, true, left);
    buttonDriver_addHandler(4, true, floatReturnCall);
    buttonDriver_addHandler(1, true, backFunction);
}

void numberEditor_initInt(uint32_t oldValue, void (*returnFunction)(uint32_t), handler backFunction,
                          char *valueName, char *valueLabel, uint32_t min, uint32_t max) {
    statusDriverLine_setStatus(false);
    floatFunc = false;
    col = 3;
    myInt = oldValue;
    returnIntFunction = returnFunction;
    displayDriver_disableCurser();
    copyTitle(valueName);
    minValue = min;
    maxValue = max;
    displayDriver_clearDispaly();
    displayDriver_tick();
    displayDriver_writeString(title);
    char myString[7];
    sprintf(myString, "%04d", myInt);
    displayDriver_setCurserPos(2, START_COL);
    displayDriver_writeString(myString);
    displayDriver_writeChar(' ');
    displayDriver_writeString(valueLabel);
    setEditColumn(START_COL + MAX_EDIT_COL_INT);
    stepValue = 1;
    buttonDriver_clearAllHandlers();
    buttonDriver_addHandler(3, true, increment);
    buttonDriver_addHandler(5, true, decrement);
    buttonDriver_addHandler(2, true, right);
    buttonDriver_addHandler(6, true, left);
    buttonDriver_addHandler(4, true, intReturnCall);
    buttonDriver_addHandler(1, true, backFunction);
}

void numberEditor_tick();