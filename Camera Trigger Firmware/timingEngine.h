/*
 * timingEngine.h
 *
 * Created: 7/29/2020 4:44:00 PM
 *  Author: chadb
 */

#ifndef TIMINGENGINE_H_
#define TIMINGENGINE_H_
#define TIMINGENGINE_TICK_TIME 0.02
#define TIMINGENGINE_TICKS_PER_100_MILLIS 5

void timingEngine_init();
void timingEngine_tick();
void timingEngine_loadEvents();
void timingEngine_runEvents();
void timingEngine_loadAndRunEvents();

#endif /* TIMINGENGINE_H_ */