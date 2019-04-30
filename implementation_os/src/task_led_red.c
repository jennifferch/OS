#include "os.h"
#include "sapi.h"
#include "task_led_red.h"

uint8_t stackLedRed[STACK_SIZE];

extern event_t ledRedEvent;
extern osTicks_t tTotal;

void taskLedRed(void *a) {
   while(1) {
	   os_event_wait(&ledRedEvent);
      
      gpioWrite(LED2, ON);
      set_delay_os(tTotal);
      gpioWrite(LED2, OFF);
   }
}
