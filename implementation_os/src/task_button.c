#include "os.h"
#include "sapi.h"
#include "task_button.h"

uint8_t stack_button[STACK_SIZE];

void task_button(void *a) {
   while(1) {
      gpioWrite(LEDG, !gpioRead(TEC1));
   }
 }
