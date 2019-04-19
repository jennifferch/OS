#ifndef MY_RTOS_CONFIG_H
#define MY_RTOS_CONFIG_H

#include "stdint.h"
#include "os_events.h"

/*==================[macros and definitions]=================================*/

// Amount of priority levels
#define OS_PRIORITY_LEVELS 4

#define EXCHANGE_DELAY      6
#define SAMPLES          240
/*==================[internal data declaration]==============================*/
event_t processEvent;
event_t fillEvent;

uint16_t bufferA[SAMPLES];
uint16_t bufferB[SAMPLES];
/*==================[internal functions declaration]=========================*/

#endif
