#include "os.h"
#include "sapi.h"
#include "task_led_yellow.h"

uint8_t stackLedYellow[STACK_SIZE];

extern event_t ledYellowEvent;
extern osTicks_t tTotal;

void taskLedYellow(void *a) {
   while(1) {
	   os_event_wait(&ledYellowEvent);
      
      gpioWrite(LED1, ON);
      set_delay_os(tTotal);
      gpioWrite(LED1, OFF);
   }
}
