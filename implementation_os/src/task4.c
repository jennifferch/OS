#include "os.h"
#include "sapi.h"
#include "task4.h"

uint8_t stack4[STACK_SIZE];

void * task4_button(void *a) {
	 while(1) {
	      gpioWrite(LEDB, !gpioRead(TEC1));
	   }
	 return NULL;

}
