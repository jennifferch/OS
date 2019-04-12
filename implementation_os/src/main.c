/*==================[inclusions]=============================================*/

#include "main.h"
#include "sapi.h"
#include "os.h"

#include "task1.h"
#include "task2.h"
#include "task3.h"
#include "task4.h"
#include "task5.h"
#include "task6.h"
#include "task_button.h"

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

   /* Start the os */
   os_start();

   while (1) {}
}

blinkTaskData_t task3_blink = { .delay = 250, .led = LED1 };
blinkTaskData_t task5_blink = { .delay = 500, .led = LED2 };
blinkTaskData_t task6_blink = { .delay = 100, .led = LED3 };

#define USER_TASKS                                                           \
	   OS_INIT_TASK(task3, TASK_PRIORITY_LOW, (uint32_t *)stack3, STACK_SIZE, &task3_blink)     \
       OS_INIT_TASK(task5, TASK_PRIORITY_HIGH, (uint32_t *)stack5, STACK_SIZE, &task5_blink)     \
       OS_INIT_TASK(task6, TASK_PRIORITY_HIGH, (uint32_t *)stack6, STACK_SIZE, &task6_blink)

taskControl_t tasks_list[] = {
	  USER_TASKS
	  LAST_TASK
};


/** @} doxygen end group definition */

/*==================[end of file]============================================*/
