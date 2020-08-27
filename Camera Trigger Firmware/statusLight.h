/*
 * statusLight.h
 *
 * Created: 7/21/2020 3:07:35 PM
 *  Author: chadb
 */ 


#ifndef STATUSLIGHT_H_
#define STATUSLIGHT_H_

void statusLight_init();
void statusLight_setNominal();
void statusLight_setError(unsigned char blinkPattern);
void statusLight_tick();



#endif /* STATUSLIGHT_H_ */