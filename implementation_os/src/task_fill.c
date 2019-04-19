#include "os.h"
#include "sapi.h"
#include "task_fill.h"

uint32_t fillStack[STACK_SIZE];

void task_fill(void *a) {

	blinkTaskData_t *myConfig = (blinkTaskData_t *) a;
	uint16_t * buffer = bufferA;
	uint32_t i;
	while (TRUE) {

		os_event_wait(&fillEvent);

		gpioToggle(myConfig->led);

		for (i = 0; i < SAMPLES; i++) {
			buffer[i] = i + 1;
		}
		if (buffer == bufferA) {
			buffer = bufferB;
		} else {
			buffer = bufferA;
		}

	}
}
