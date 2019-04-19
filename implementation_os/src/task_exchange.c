#include "os.h"
#include "sapi.h"
#include "task_exchange.h"

uint32_t exchangeStack[STACK_SIZE];

void task_exchange(void *a) {

	uint8_t ping = 1;

	while (TRUE) {

		if (!ping) {
			os_event_set(&processEvent);
		}
		ping = 0;
		os_event_set(&fillEvent);
		set_delay_os(6);
	}
}
