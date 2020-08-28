#include "events.h"

uint32_t getEventTiming(Event event) {
    return event.data.value;
}
