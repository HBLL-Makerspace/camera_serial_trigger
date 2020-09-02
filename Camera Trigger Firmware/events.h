#include <stdint.h>

typedef struct uint24_t {
    uint32_t value : 24;
} uint24_t;

typedef struct Event {
    uint8_t type;
    uint24_t data;
} Event;

typedef union EventMemoryConverter {
    uint32_t memoryData;
    Event event;
} EventMemoryConverter;

void events_updateEventCount(uint8_t count);

uint8_t events_eventCount();

void events_loadEventsFromEEPROM();

void events_saveEventsToEEPROM();

Event events_getEvent(uint8_t eventIndex);

void events_setEvent(uint8_t eventIndex, Event event);

void events_resetEvents();
