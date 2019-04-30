#include <string.h>
#include "os.h"
#include "chip.h"

///////////////////////////////////////////////////////////////////////////////
// Internal functions declarations
static bool init_task(taskControl_t *task);

static bool get_ready_task_os(taskID_t *id);
static void set_state_task(taskControl_t * task);
static void update_ready_taskList_os(taskID_t id);

///////////////////////////////////////////////////////////////////////////////
// Extern task
///////////////////////////////////////////////////////////////////////////////
extern taskControl_t tasks_list[OS_MAX_TASK];

///////////////////////////////////////////////////////////////////////////////
// Internal data definitions
///////////////////////////////////////////////////////////////////////////////

static uint8_t idleStack[STACK_SIZE];

osState_t osState = OS_STATE_TASK;

bool osSwitchRequired;

///////////////////////////////////////////////////////////////////////////////
// internal functions definition
///////////////////////////////////////////////////////////////////////////////
/**
 * It is executed when no task is in ready state.
 */
__attribute__ ((weak)) void idle_hook(void * parameters) {
	while (1) {
		/* Sleep */
		__WFI();
	}
}

/**
 * idle task control structure.
 */
static taskControl_t idleTaskControl = { .entryPoint = idle_hook /* entryPoint of the task */
, .priority = TASK_PRIORITY_IDLE /* priority of the task idle */
, .stackSize = STACK_SIZE /* size of the stack  */
, .stack = (uint32_t *) idleStack /* initialization of the stack */
, .state = TASK_STATE_READY /* Initialization in 0 */
, .stackPointer = 0 /* Initialization in 0 */
, .initialParameter = 0 /* Initialization in 0 */
};

/**
 * OS control structure.
 */
static osControl_t control_task_os = { .numTask = 0 /* Initialization in 0 */
, .currentTask = OS_ACTUAL_TASK_NONE /* Initialization in invalid value */
, .tickCount = 0 /* Initialization in 0  */
, .state = OS_STATE_TASK /* Initialization in OS_STATE_TASK  */
};

///////////////////////////////////////////////////////////////////////////////
// External functions definitions
///////////////////////////////////////////////////////////////////////////////

int32_t critical_counter = 0;

void os_enter_critical(void) {
	critical_counter++;
	__disable_irq();
}

void os_exit_critical(void) {
	critical_counter--;
	if (critical_counter <= 0) {
		critical_counter = 0;
		__enable_irq();
	}
}

/**
 * starts the OS.
 */
void os_start(void) {

	uint32_t i;

	memset(idleStack, 0, STACK_SIZE);
	memset(control_task_os.readyTaskList, OS_ACTUAL_TASK_NONE,
			(OS_MAX_TASK + 1) * OS_PRIORITY_LEVELS * sizeof(osControl_t*));

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
			add_ready_task_os(control_task_os.numTask);

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
 * Function set the delay in the task,
 * then sets the status task to blocked
 * and runs the scheduler
 */
void set_delay_os(uint32_t milliseconds) {

	  /* Check if valid calling context */
	   if (osState == OS_STATE_IRQ) {
	      return;
	   }


	os_enter_critical();
	if (control_task_os.currentTask != OS_ACTUAL_TASK_NONE
			&& milliseconds != 0) {
		control_task_os.taskList[control_task_os.currentTask].delay =
				milliseconds;
		set_blocked_task_os(control_task_os.currentTask);
		os_exit_critical();
		schedule();
	} else {
		os_exit_critical();
	}
}

/**
 * next context selection routine to be executed.
 */
uint32_t getNextContext(uint32_t currentSP) {

	taskID_t findNextTask = OS_ACTUAL_TASK_NONE;
	/* The first time return the stack pointer of the the first task of the highest priority */

	if (control_task_os.currentTask != ACTUAL_TASK_MAIN) {
		/* Save current task stack pointer*/
		control_task_os.taskList[control_task_os.currentTask].stackPointer =
				currentSP;

		/* Update the current task's list*/
		update_ready_taskList_os(control_task_os.currentTask);
	}

	/* Mark the task as 'Ready'*/
	set_state_task(&control_task_os.taskList[control_task_os.currentTask]);

	/* Get the next higher priority 'Ready' task*/
	if (get_ready_task_os(&findNextTask)) {
		control_task_os.currentTask = findNextTask;
	} else {
		/* Set the current task with the idle task */
		control_task_os.currentTask = control_task_os.numTask;
	}

	/* Mark the task as 'Running'*/
	set_state_task(&control_task_os.taskList[control_task_os.currentTask]);

	/* Return new stack pointer*/
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

/*
 * Change task's state to blocked
 */
void set_blocked_task_os(taskID_t id) {

	control_task_os.taskList[id].state = TASK_STATE_BLOCKED;
}

/**
 * Add a task to the end of the list list according to its priority and
 * change your status to ready
 */
void add_ready_task_os(taskID_t id) {

	uint32_t pos = 0;
	taskPriority_t priority = control_task_os.taskList[id].priority;

	/* Change task's state to ready*/
	control_task_os.taskList[id].state = TASK_STATE_READY;

	/* Finds an empty position within the list of the given task priority*/
	while (control_task_os.readyTaskList[priority][pos] != OS_ACTUAL_TASK_NONE
			&& pos < OS_MAX_TASK + 1) {
		pos++;
	}

	/* If an empty position was found the task is saved there*/
	if (pos < OS_MAX_TASK + 1) {
		control_task_os.readyTaskList[priority][pos] = id;
	}
}

/**
 * Returns true if it finds the first task to execute
 * within the list of all priorities
 */
static bool get_ready_task_os(taskID_t *id) {

	uint32_t firstPos = 0;
	taskPriority_t p;

	for (p = TASK_PRIORITY_HIGH; p < TASK_PRIORITY_IDLE; p++) {

		if (control_task_os.readyTaskList[p][firstPos] != OS_ACTUAL_TASK_NONE) {
			*id = control_task_os.readyTaskList[p][firstPos];
			return true;
		}

	}

	/* No ready task was found*/
	return false;
}

/**
 * Update the list of tasks listed by priorities.
 * Set the present task at the end of the tasks
 * within the list or is removed.
 */
static void update_ready_taskList_os(taskID_t id) {
	/* The position 0 is to be used by the running task*/
	uint32_t pos = 0;

	/* Check if it is a valid task*/
	if (id == OS_ACTUAL_TASK_NONE) {
		return;
	}

	taskPriority_t priority = control_task_os.taskList[id].priority;

	/* Verify that the passed task is the running task on its list*/
	if (control_task_os.readyTaskList[priority][pos] != id) {
		return;
	}

	/*go through the list moving the 'ready' tasks to the top within your priority*/
	do {
		pos++;
		control_task_os.readyTaskList[priority][pos - 1] =
				control_task_os.readyTaskList[priority][pos];
	} while (control_task_os.readyTaskList[priority][pos] != OS_ACTUAL_TASK_NONE
			&& pos < OS_MAX_TASK + 1);

	/* If the current task is still running, it moves to the bottom */

	if (control_task_os.taskList[id].state == TASK_STATE_RUNNING) {
		control_task_os.readyTaskList[priority][pos - 1] = id;
	}
}

/**
 * Function if a task has return
 */
static void return_hook(void * returnValue) {

	while (1)
		;
}

/**
 * Create the initial context
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
 *
 * @return current system ticks
 */
osTicks_t get_systemTicks_os(void) {

   return control_task_os.tickCount;
}

/**
 * Function that updates the remaining ticks of blocked tasks,
 * if the delay is 0 then the state of task is updated.
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
				add_ready_task_os(i);
			}
		}
	}
}

/**
 * Function that return the current task
 */
taskID_t get_current_task(void) {

	return control_task_os.currentTask;
}

/**
 * Function that return the state of so
 */
osState_t get_state_os(void) {

	return control_task_os.state;
}

/**
 * schedule
 */
void schedule(void) {

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

