#include "os_task.h"
#include "sapi.h"
#include "task1.h"

uint8_t stack1[STACK_SIZE];

void * task1(void *a) {
   uint32_t i;
   gpioToggle(LED3);
   while(1) {
      for (i = 0; i < 500000; i++) {}
      gpioToggle(LED3);
   }

   return NULL;
}
