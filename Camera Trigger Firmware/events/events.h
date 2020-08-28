#include <stdint.h>

struct uint24_t {
    uint32_t value : 24;
};

typedef struct Event {
    uint8_t type;
    uint24_t data;
} Event;

typedef union EventMemoryConverter {
    uint32_t memoryData;
    Event event;
} EventMemoryConverter;

uint32_t getEventTiming(Event event);
