/*
 * numberEditor.h
 *
 * Created: 8/6/2020 3:29:55 PM
 *  Author: chadb
 */ 


#ifndef NUMBEREDITOR_H_
#define NUMBEREDITOR_H_

#include "globalVar.h"

void numberEditor_initFloat(float oldValue, void (*returnFunction)(float), handler backFunction, char* valueName, char* valueLabel, float min, float max);
void numberEditor_initInt(uint8_t oldValue, void (*returnFunction)(uint8_t), handler backFunction, char* valueName, char* valueLabel, uint8_t min, uint8_t max);
void numberEditor_tick();




#endif /* NUMBEREDITOR_H_ */