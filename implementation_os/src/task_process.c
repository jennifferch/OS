#include "os.h"
#include "sapi.h"
#include "task_process.h"

uint32_t processStack[STACK_SIZE];

void task_process(void *a) {

	blinkTaskData_t *myConfig = (blinkTaskData_t *) a;
	uint16_t * buffer = bufferA;
	uint32_t i;
	while (TRUE) {

		os_event_wait(&processEvent);

		gpioToggle(myConfig->led);

		for (i = 0; i < SAMPLES; i++) {
			buffer[i] *= 2;
		}
		if (buffer == bufferA) {
			buffer = bufferB;
		} else {
			buffer = bufferA;
		}

	}
}
