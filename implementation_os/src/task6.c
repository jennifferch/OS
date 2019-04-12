#include "os.h"
#include "sapi.h"
#include "task6.h"

uint8_t stack6[STACK_SIZE];

void task6(void *config) {
	blinkTaskData_t *myConfig = (blinkTaskData_t *) config;

	while (1) {
		gpioToggle(myConfig->led);
		set_delay_os(myConfig->delay);
	}
}
