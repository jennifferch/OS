#include <string.h>
#include "os.h"
#include "chip.h"

///////////////////////////////////////////////////////////////////////////////
// Internal functions declarations
static bool init_task(taskControl_t *task);
static void schedule(void);

static int32_t get_ready_task_os(taskPriority_t prio);
static void set_state_task(taskControl_t * task);
static int32_t get_priority_task_os(void);

///////////////////////////////////////////////////////////////////////////////
// Extern task
extern taskControl_t tasks_list[OS_MAX_TASK];

///////////////////////////////////////////////////////////////////////////////
// Internal data definitions

static uint8_t idleStack[STACK_SIZE];

///////////////////////////////////////////////////////////////////////////////
// internal functions definition

/**
 * It is executed when no task is in ready state.
 */
__attribute__ ((weak)) void idle_hook(void * parameters) {
	while (1) {
		/* Sleep */
		__WFI();
	}
}

static taskControl_t idleTaskControl = { .entryPoint = idle_hook /* entryPoint of the task */
, .priority = TASK_PRIORITY_IDLE /* priority of the task */
, .stackSize = STACK_SIZE /* size of the stack  */
, .stack = (uint32_t *) idleStack /* initialization of the stack */
, .state = TASK_STATE_READY /* Initialization in 0 */
, .stackPointer = 0 /* Initialization in 0 */
, .initialParameter = 0 /* Initialization in 0 */
};

static osControl_t control_task_os = { .numTask = 0 /* Initialization in 0 */
, .currentTask = OS_ACTUAL_TASK_NONE /* Initialization in invalid value */
, .tickCount = 0 /* Initialization in 0  */
};
///////////////////////////////////////////////////////////////////////////////

/**
 * starts the OS.
 */
void os_start(void) {
	uint32_t i;

	memset(idleStack, 0, STACK_SIZE);
	taskControl_t * t = tasks_list;

	// init task check the entry point
	while (t->entryPoint != 0) {

		/* set all the tasks of the user in the OS task control list */
		if (OS_MAX_TASK > control_task_os.numTask) {
			control_task_os.taskList[control_task_os.numTask].entryPoint =
					t->entryPoint;
			control_task_os.taskList[control_task_os.numTask].priority =
					t->priority;
			control_task_os.taskList[control_task_os.numTask].stack = t->stack;
			control_task_os.taskList[control_task_os.numTask].stackPointer =
					t->stackPointer;
			control_task_os.taskList[control_task_os.numTask].stackSize =
					t->stackSize;
			control_task_os.taskList[control_task_os.numTask].state = t->state;
			control_task_os.taskList[control_task_os.numTask].initialParameter =
					t->initialParameter;
			control_task_os.taskList[control_task_os.numTask].delay = t->delay;

			// init task user
			init_task(&control_task_os.taskList[control_task_os.numTask]);
			control_task_os.numTask++;
		}
		t++;
	}

	/* set the idle task in the in the OS task control list */
	control_task_os.taskList[control_task_os.numTask] = idleTaskControl;
	// init idle task task
	init_task(&control_task_os.taskList[control_task_os.numTask]);

	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);

	//set PendSV with the lowest priority
	NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
}

/*
 *
 * function set the delay in the task and set the state in blocked
 */
void set_delay_os(uint32_t milliseconds) {

	if (control_task_os.currentTask != OS_ACTUAL_TASK_NONE
			&& milliseconds != 0) {
		control_task_os.taskList[control_task_os.currentTask].delay =
				milliseconds;
		control_task_os.taskList[control_task_os.currentTask].state =
				TASK_STATE_BLOCKED;
		schedule();
	}
}

/**
 * next context selection routine to be executed.
 */
uint32_t getNextContext(uint32_t currentSP) {

	/* The first time return the stack pointer of the the first task of the highest priority */
	if (control_task_os.currentTask == ACTUAL_TASK_MAIN) {
		control_task_os.currentTask = get_priority_task_os();
		return control_task_os.taskList[control_task_os.currentTask].stackPointer;
	}

	/* Save current task stack pointer*/
	control_task_os.taskList[control_task_os.currentTask].stackPointer =
			currentSP;

	/* Mark the task as 'Ready'*/
	set_state_task(&control_task_os.taskList[control_task_os.currentTask]);

	taskPriority_t p;
	bool_t findValue = true;

	/* For each priority find the task in state 'Ready'
	 * Higher priority == Less number
	 */
	for (p = TASK_PRIORITY_HIGH; p > TASK_PRIORITY_IDLE; p--) {
		// find 'Ready' task to be executed
		int32_t findTask = get_ready_task_os(p);
		if (findTask >= 0) {
			control_task_os.currentTask = findTask;
			break;
		}
	}

	/* If we leave the for without finding a task ready */
	if (p == TASK_PRIORITY_IDLE) {
		/* Set the current task with the idle task */
		control_task_os.currentTask = control_task_os.numTask;
	}

	// Mark the task as 'Running'
	set_state_task(&control_task_os.taskList[control_task_os.currentTask]);

	// Return new stack pointer
	return control_task_os.taskList[control_task_os.currentTask].stackPointer;
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

/* if a task has return*/
static void return_hook(void * returnValue) {

	while (1)
		;
}

/**
 * Find task in 'Ready' state
 *
 */
static int32_t get_ready_task_os(taskPriority_t prio) {
	uint32_t i;

	//Find upper part of the list.
	for (i = control_task_os.currentTask + 1; i < control_task_os.numTask;
			i++) {
		if (control_task_os.taskList[i].state == TASK_STATE_READY
				&& control_task_os.taskList[i].priority == prio) {
			return i;
		}
	}

	i = 0;

	//Find Lower part of the list.
	while (i <= control_task_os.currentTask) {
		if (control_task_os.taskList[i].state == TASK_STATE_READY
				&& control_task_os.taskList[i].priority == prio) {
			return i;
		}
		i++;

	}
	return -1;
}

/**
 * Find task in 'Ready' state
 *
 */
static int32_t get_priority_task_os(void) {
	uint32_t i = 0;
	taskPriority_t p;

	for (p = TASK_PRIORITY_HIGH; p > TASK_PRIORITY_IDLE; p--) {
		for (i = 0; i < control_task_os.numTask; i++) {
			if (control_task_os.taskList[i].state == TASK_STATE_READY
					&& control_task_os.taskList[i].priority == p) {
				return i;
			}
		}
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

	uint32_t i;

	control_task_os.tickCount++;

	for (i = 0; i < control_task_os.numTask; i++) {
		if (control_task_os.taskList[i].state == TASK_STATE_BLOCKED
				&& control_task_os.taskList[i].delay != 0) {
			control_task_os.taskList[i].delay--;
			if (control_task_os.taskList[i].delay == 0) {
				control_task_os.taskList[i].state = TASK_STATE_READY;
			}
		}
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

