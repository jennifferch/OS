/*==================[inclusions]=============================================*/

#include "main.h"
#include "sapi.h"
#include "os.h"

#include "task3.h"
#include "task5.h"
#include "task6.h"
#include "task_button.h"
#include "os_events.h"
#include "task_process.h"
#include "task_fill.h"
#include "task_exchange.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/**
 *	hardware initialization function
 */
static void initHardware(void);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/


static void initHardware(void) {
   boardConfig();
}


/*==================[external functions definition]==========================*/

int main(void) {
   initHardware();

   /* Start the events */
   os_event_init(&processEvent);
   os_event_init(&fillEvent);

   /* Start the os */
   os_start();

   while (1) {}
}

blinkTaskData_t task3_blink = { .delay = 250, .led = LED1 };
blinkTaskData_t task5_blink = { .delay = 500, .led = LED2 };
blinkTaskData_t task6_blink = { .delay = 100, .led = LED3 };
/*
blinkTaskData_t task3_blink = { .delay = 2, .led = LED1 };
blinkTaskData_t task5_blink = { .delay = 3, .led = LED2 };
blinkTaskData_t task6_blink = { .delay = 4, .led = LED3 };

#define USER_TASKS                                                           \
	   OS_INIT_TASK(task3, TASK_PRIORITY_MEDIUM, (uint32_t *)stack3, STACK_SIZE, &task3_blink)     \
       OS_INIT_TASK(task5, TASK_PRIORITY_HIGH, (uint32_t *)stack5, STACK_SIZE, &task5_blink)     \
       OS_INIT_TASK(task_button, TASK_PRIORITY_LOW, (uint32_t *)stack_button, STACK_SIZE, &task6_blink)*/


#define USER_TASKS                                                           \
	   OS_INIT_TASK(task_exchange, TASK_PRIORITY_HIGH, (uint32_t *)exchangeStack, STACK_SIZE, &task3_blink)     \
       OS_INIT_TASK(task_fill, TASK_PRIORITY_MEDIUM, (uint32_t *)fillStack, STACK_SIZE, &task5_blink)     \
       OS_INIT_TASK(task_process, TASK_PRIORITY_MEDIUM, (uint32_t *)processStack, STACK_SIZE, &task6_blink)

taskControl_t tasks_list[] = {
	  USER_TASKS
	  LAST_TASK
};


/** @} doxygen end group definition */

/*==================[end of file]============================================*/
