#include "os.h"
#include "sapi.h"
#include "task3.h"

uint8_t stack3[STACK_SIZE];

void * task3(void *a) {
 /*  uint32_t j;

   while(1) {
     for (j = 0; j < 2000000; j++) {}
     gpioToggle(LED1);
   }
   return NULL;*/

	 blinkTaskData_t *myConfig = (blinkTaskData_t *) a;

	 	while (1) {
	 		gpioToggle(myConfig->led);
	 		set_delay_os(myConfig->delay);
	 	}
	 	 return NULL;
}
