#include <string.h>
#include "chip.h"
#include "os.h"
#include "os_task.h"
#include "user_tasks.h"

///////////////////////////////////////////////////////////////////////////////

// Tasks list of OS
#define OS_INIT_TASK(_entryPoint, _stack, _stackSize, _parameter)        \
      {.entryPoint = _entryPoint, .stack = _stack, .stackPointer = 0,         \
      .stackSize = _stackSize, .state = TASK_READY,                           \
      .initialParameter = _parameter},

static taskControl_t tasks_list[] = {
	  USER_TASKS
	  LAST_OS_TASK
};

#undef OS_INIT_TASK

///////////////////////////////////////////////////////////////////////////////

static bool init_task(taskControl_t *task);

///////////////////////////////////////////////////////////////////////////////

extern taskControl_t tasks_list[];

///////////////////////////////////////////////////////////////////////////////

/**
 * starts the OS.
 */
void start_os(void) {

   taskControl_t * t = tasks_list;

   // init task check the entry point
   while(t->entryPoint != 0) {
	   init_task(t);
      t++;
   }

   SystemCoreClockUpdate();
   SysTick_Config(SystemCoreClock / 1000);

   //configuro PendSV con la prioridad más baja
   NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
}


/**
 * next context selection routine to be executed.
 */
uint32_t getNextContext(uint32_t currentSP) {
   static int32_t current_task = ACTUAL_TASK_MAIN;

   // The first time return the task1 stack pointer.
   if (current_task == ACTUAL_TASK_MAIN) {
      current_task = 0;
      return tasks_list[current_task].stackPointer;
   }

   // Save current task stack pointer.
   tasks_list[current_task].stackPointer = currentSP;

   // Increment the index.
   current_task++;

   // Check if is the last task {0,0,0,0,0}
   if (tasks_list[current_task].entryPoint == 0) {
      current_task = 0;
   }

   // Return new stack pointer
   return tasks_list[current_task].stackPointer;
}

/* if a task has return*/
static void return_hook(void * returnValue)
{
	while(1);
}

/**
 * create the initial context
 */
static bool init_task(taskControl_t *task) {
   // Initialize stack with 0.
   memset(task->stack, 0, task->stackSize);

   // Initialization stack pointer.
   task->stackPointer = (uint32_t)&(task->stack[(task->stackSize / 4) - 17]);

   // Indicate ARM/Thumb mode in PSR registers.
   task->stack[(task->stackSize / 4) - 1] = INITIAL_xPSR;

   // Program counter is the pointer to task
   task->stack[(task->stackSize / 4) - 2] = (uint32_t)task->entryPoint;

   // LR - Link Register. to return hook.
   task->stack[(task->stackSize / 4) - 3] = (uint32_t)return_hook;

   // R0 - First parameter passed to the task. 0 for now.
   task->stack[(task->stackSize / 4) - 8] = (uint32_t)task->initialParameter;

   // lr from stack.
   task->stack[(task->stackSize / 4) - 9] = EXC_RETURN;

   return true;
}

/**
 * SysTick Handler
 */
void SysTick_Handler(void) {
   // Activate PendSV for context switching
   SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;

   // Instruction Synchronization Barrier: this way we make sure that all
   // pipelined instructions are executed
   __ISB();
      
   // Data Synchronization Barrier: this way we make sure that all
   // memory accesses are completed
   __DSB();
}
