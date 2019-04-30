#include "os.h"
#include "sapi.h"
#include "task_led_green.h"

uint8_t stackLedGreen[STACK_SIZE];

extern event_t ledGreenEvent;
extern osTicks_t tTotal;

void taskLedGreen(void *a) {
   while(1) {
	   os_event_wait(&ledGreenEvent);
      
      gpioWrite(LED3, ON);
      set_delay_os(tTotal);
      gpioWrite(LED3, OFF);
   }
}
