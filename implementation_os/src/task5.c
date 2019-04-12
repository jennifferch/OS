#include "task5.h"

#include "../../../cygwin/usr/include/stdint.h"
#include "../../libs/sapi/sapi_v0.5.2/soc/peripherals/inc/sapi_gpio.h"
#include "../inc/os.h"

uint8_t stack5[STACK_SIZE];

void task5(void *a) {
	blinkTaskData_t *myConfig = (blinkTaskData_t *) a;

	while (1) {
		gpioToggle(myConfig->led);
		set_delay_os(myConfig->delay);
	}
}
