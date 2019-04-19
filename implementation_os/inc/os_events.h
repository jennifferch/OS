#ifndef OS_EVENTS_H
#define OS_EVENTS_H

#include "stdint.h"
#include "stdbool.h"
#include "os.h"

typedef uint32_t eventID_t;
typedef int32_t taskID_t;

typedef enum {
   EVENT_UNINITIALIZED = 0,
   EVENT_INITIALIZED = 1,
   EVENT_WAITING = 2,
} eventState_t;

typedef struct {
   taskID_t taskId;
   eventID_t eventId;
   eventState_t state;
} event_t;

bool os_event_init(event_t *event);

bool os_event_wait(event_t *event);

bool os_event_set(event_t *event);

#endif
