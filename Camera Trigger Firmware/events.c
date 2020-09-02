#include "events.h"
#include "eepromDriver.h"
#include "globalVar.h"
#include "usart.h"
#include "usart0.h"

static Event events[256]; /// Create array of events
static uint8_t eventCount;

void events_loadEventsFromEEPROM() {
    eventCount = eepromDriver_readDword(GLOBALVAR_TIMING_SET_COUNT);
    EventMemoryConverter _converter;
    for (uint8_t i = 0; i < eventCount; i++) {
        _converter.memoryData = (eepromDriver_readDword(GLOBALVAR_TIMING_BEGIN_MEM_SLOT + i));
        events[i] = _converter.event;
        usart0_send("event loaded");
    }
}

void events_saveEventsToEEPROM() {
    eepromDriver_writeDword(GLOBALVAR_TIMING_SET_COUNT, eventCount);
    EventMemoryConverter _converter;
    for (uint8_t i = 0; i < eventCount; i++) {
        _converter.event = events[i];
        eepromDriver_writeDword(GLOBALVAR_TIMING_BEGIN_MEM_SLOT + i, _converter.memoryData);
    }
}

void events_updateEventCount(uint8_t count) {
    eventCount = count;
    eepromDriver_writeDword(GLOBALVAR_TIMING_SET_COUNT, eventCount);
}

uint8_t events_eventCount() { return eventCount; }

Event events_getEvent(uint8_t eventIndex) {
    if (eventIndex < eventCount)
        return events[eventIndex];
    else {
        usart0_send("Failed to get event.");
        return events[0];
    }
}

void events_setEvent(uint8_t eventIndex, Event event) {
    if (eventIndex < eventCount) {
        events[eventIndex] = event;
    } else {
        usart0_send("Failed to set event");
    }
}

void events_resetEvents() {
    eepromDriver_writeDword(GLOBALVAR_TIMING_SET_COUNT, 6);
    for (uint8_t i = 0; i < 6; i++) {
        eepromDriver_writeDword(GLOBALVAR_TIMING_BEGIN_MEM_SLOT + i, 0);
    }
}
