/*
 * shutterDriver.h
 *
 * Created: 7/29/2020 1:17:33 PM
 *  Author: chadb
 */ 


#ifndef SHUTTERDRIVER_H_
#define SHUTTERDRIVER_H_

void shutterDriver_init();
void shutterDriver_enableChanAFocus();
void shutterDriver_enableChanBFocus();
void shutterDriver_enableChanAShutter();
void shutterDriver_enableChanBShutter();
void shutterDriver_disableChanAFocus();
void shutterDriver_disableChanBFocus();
void shutterDriver_disableChanAShutter();
void shutterDriver_disableChanBShutter();
void shutterDriver_enableAllFocus();
void shutterDriver_enableAllShutter();
void shutterDriver_disableAllFocus();
void shutterDriver_disableAllShutter();
void shutterDriver_toggleFocus();


#endif /* SHUTTERDRIVER_H_ */