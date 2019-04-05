#include <string.h>
#include "os.h"
#include "chip.h"

///////////////////////////////////////////////////////////////////////////////
// Internal functions declarations
static bool init_task(taskControl_t *task);
static void idle_hook(void * p);
static void schedule(void);

static int32_t get_ready_task_os(void);
static void set_state_task(taskControl_t * task);

///////////////////////////////////////////////////////////////////////////////
// Extern task
extern taskControl_t tasks_list[];

///////////////////////////////////////////////////////////////////////////////
// Internal data definitions
static int32_t currentTask = OS_ACTUAL_TASK_NONE;

static uint8_t idleStack[STACK_SIZE];

static taskControl_t idleTaskControl = { .entryPoint = idle_hook, .stackSize =
STACK_SIZE, .stack = (uint32_t *) idleStack, .state = TASK_STATE_READY,
		.stackPointer = 0, .initialParameter = 0 };
///////////////////////////////////////////////////////////////////////////////

/**
 * starts the OS.
 */
void os_start(void) {

	memset(idleStack, 0, STACK_SIZE);
	taskControl_t * t = tasks_list;

	// init task check the entry point
	while (t->entryPoint != 0) {
		init_task(t);
		t++;
	}

	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);

	//configuro PendSV con la prioridad más baja
	NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
}

void set_delay_os(uint32_t milliseconds) {

	if (currentTask != OS_ACTUAL_TASK_NONE && milliseconds != 0) {
		tasks_list[currentTask].delay = milliseconds;
		tasks_list[currentTask].state = TASK_STATE_BLOCKED;
		schedule();
	}
}

/**
 * next context selection routine to be executed.
 */
uint32_t getNextContext(uint32_t currentSP) {

	if (tasks_list[0].entryPoint == 0) {
		return idleTaskControl.stackPointer;
	}

	// The first time return the task1 stack pointer.
	if (currentTask == ACTUAL_TASK_MAIN) {
		currentTask = 0;
		return tasks_list[currentTask].stackPointer;
	}

	// Save current task stack pointer.
	tasks_list[currentTask].stackPointer = currentSP;

	// Mark the task as 'Ready'
	set_state_task(&tasks_list[currentTask]);

	// find 'Ready' task to be executed
	currentTask = get_ready_task_os();
	if (currentTask < 0) {
		return idleTaskControl.stackPointer;
	}

	// Mark the task as 'Running'
	set_state_task(&tasks_list[currentTask]);

	// Return new stack pointer
	return tasks_list[currentTask].stackPointer;
}

/**
 * State machine for tasks.
 */
static void set_state_task(taskControl_t * task) {

	switch (task->state) {

	case TASK_STATE_READY:

		task->state = TASK_STATE_RUNNING;
		break;
	case TASK_STATE_RUNNING:

		task->state = TASK_STATE_READY;
		break;
	case TASK_STATE_BLOCKED:

		break;
	case TASK_STATE_ERROR:

		break;
	case TASK_STATE_TERMINATED:

		break;
	default:

		break;
	}
}

static void idle_hook(void * p) {

	while (1) {
		__WFI();
	}

}

/* if a task has return*/
static void return_hook(void * returnValue) {

	while (1)
		;
}

/**
 * Find task in 'Ready' state
 *
 */
static int32_t get_ready_task_os(void) {
	uint32_t i = currentTask + 1;

	//Find upper part of the list.
	while (tasks_list[i].entryPoint != 0) {
		if (tasks_list[i].state == TASK_STATE_READY) {
			return i;
		}
		i++;
	}

	i = 0;

	//Find Lower part of the list.
	while (i <= currentTask) {
		if (tasks_list[i].state == TASK_STATE_READY) {
			return i;
		}
		i++;
	}

	return -1;
}

/**
 * create the initial context
 */
static bool init_task(taskControl_t *task) {
	// Initialize stack with 0.
	memset(task->stack, 0, task->stackSize);

	// Initialization stack pointer.
	task->stackPointer = (uint32_t) &(task->stack[(task->stackSize / 4) - 17]);

	// Indicate ARM/Thumb mode in PSR registers.
	task->stack[(task->stackSize / 4) - 1] = INITIAL_xPSR;

	// Program counter is the pointer to task
	task->stack[(task->stackSize / 4) - 2] = (uint32_t) task->entryPoint;

	// LR - Link Register. to return hook.
	task->stack[(task->stackSize / 4) - 3] = (uint32_t) return_hook;

	// R0 - First parameter passed to the task. 0 for now.
	task->stack[(task->stackSize / 4) - 8] = (uint32_t) task->initialParameter;

	// lr from stack.
	task->stack[(task->stackSize / 4) - 9] = EXC_RETURN;

	return true;
}

/**
 * The delay task is updated for 'Blocked' state
 */
static void update_delay_task(void) {

	uint32_t i = 0;
	while (tasks_list[i].entryPoint != 0) {
		if (tasks_list[i].state == TASK_STATE_BLOCKED
				&& tasks_list[i].delay != 0) {
			tasks_list[i].delay--;
			if (tasks_list[i].delay == 0) {
				tasks_list[i].state = TASK_STATE_READY;
			}
		}
		i++;
	}
}

/**
 * schedule
 */
static void schedule(void) {

	// Instruction Synchronization Barrier: this way we make sure that all
	// pipelined instructions are executed
	__ISB();

	// Data Synchronization Barrier: this way we make sure that all
	// memory accesses are completed
	__DSB();

	// Activate PendSV for context switching
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;

}

/**
 * SysTick Handler
 */
void SysTick_Handler(void) {
	// Activate PendSV for context switching
	schedule();

	update_delay_task();
}

