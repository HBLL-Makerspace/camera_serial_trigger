/*
 * timingEngine.c
 *
 * Created: 7/29/2020 4:43:45 PM
 *  Author: chadb
 * 	Co-Author: Ben Brenkman
 */
#include "timingEngine.h"
#include "displayDriver.h"
#include "eepromDriver.h"
#include "events.h"
#include "globalVar.h"
#include "shutterDriver.h"
#include <stdint.h>

static enum timeEngine_t {
    init_st,
    load_next_event_st,
    delay_st,
    set_event_st,
    done_st
} engineState;

void timingEngine_init() { engineState = done_st; }

static void setEvent(Event event) {
    switch (event.type) {
    case GLOBALVAR_EN_SHUT:
        shutterDriver_enableAllShutter();
        break;
    case GLOBALVAR_EN_FOC:
        shutterDriver_enableAllFocus();
        break;
    case GLOBALVAR_DS_FOC:
        shutterDriver_disableAllFocus();
        break;
    case GLOBALVAR_DS_SHUT:
        shutterDriver_disableAllShutter();
        break;
    default:
        break;
    }
}

static Event getEvent(uint8_t eventIndex) { return events_getEvent(eventIndex); }
	
uint32_t getEventTiming(Event event) { return event.data.value; }

void timingEngine_tick() {
    static uint8_t currentEventCount = 0;
    static uint32_t counter = 0;
    static Event nextEvent;

    /// Transitions
    switch (engineState) {
    case init_st:
        engineState = load_next_event_st;
        currentEventCount = 0;
        break;
    case load_next_event_st:
        if (counter <= 1) {
            engineState = set_event_st;
        } else {
            counter--;
            engineState = delay_st;
        }
        break;
    case delay_st:
        if (counter <= 1) {
            engineState = set_event_st;
        } else {
            engineState = delay_st;
        }
        break;
    case set_event_st:
        if (currentEventCount >= events_eventCount()) {
            engineState = done_st;
        } else {
            engineState = load_next_event_st;
        }
        break;
    case done_st:
        engineState = done_st;
        break;
    default:
        engineState = init_st;
        break;
    }

    /// Actions
    switch (engineState) {
    case init_st:
        currentEventCount = 0;
        break;
    case load_next_event_st:
        nextEvent = getEvent(currentEventCount);
        counter = getEventTiming(nextEvent) * TIMINGENGINE_TICKS_PER_100_MILLIS;
        currentEventCount++;
        break;
    case delay_st:
        counter--;
        break;
    case set_event_st:
        setEvent(nextEvent);
        break;
    case done_st:
        break;
    default:
        statusLight_setError(0b10101101);
    }
}
void timingEngine_loadEvents() { events_loadEventsFromEEPROM(); }

void timingEngine_runEvents() { engineState = init_st; }

void timingEngine_loadAndRunEvents() {
    timingEngine_loadEvents();
    timingEngine_runEvents();
}
