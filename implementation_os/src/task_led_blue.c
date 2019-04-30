#include "os.h"
#include "sapi.h"
#include "task_led_blue.h"

uint8_t stackLedBlue[STACK_SIZE];

extern event_t ledBlueEvent;
extern osTicks_t tTotal;

void taskLedBlue(void *a) {
   while(1) {
	   os_event_wait(&ledBlueEvent);
      
      gpioWrite(LEDB, ON);
      set_delay_os(tTotal);
      gpioWrite(LEDB, OFF);
   }
}
