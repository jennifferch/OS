#ifndef OS_H
#define OS_H

#include "stdint.h"
#include "sapi.h"

#include "os_config.h"

/***********************************************************************/

#define LAST_TASK {0,0,0,0,0}

#define STACK_SIZE  512

#define INITIAL_xPSR    (1 << 24)

#define EXC_RETURN (0xFFFFFFF9)

#define ACTUAL_TASK_MAIN -1

#define OS_ACTUAL_TASK_NONE -1

#define OS_MAX_TASK  8

#define OS_INVALID_TASK     0xFF

#define OS_INIT_TASK(_entryPoint, _priority ,_stack, _stackSize, _parameter)        \
      {.entryPoint = _entryPoint, .priority = _priority, .stack = _stack, .stackPointer = 0,         \
      .stackSize = _stackSize, .state = TASK_STATE_READY,                           \
      .initialParameter = _parameter, .delay = 0},

/***********************************************************************/
/***********************************************************************/
typedef void (*task_t)(void *);
typedef int32_t taskID_t;
typedef uint32_t osTicks_t;

typedef enum taskState_t {
	TASK_STATE_ERROR,
	TASK_STATE_READY,
	TASK_STATE_RUNNING,
	TASK_STATE_BLOCKED,
	TASK_STATE_TERMINATED
} taskState_t;

typedef enum taskPriority_t {
	TASK_PRIORITY_HIGH,
	TASK_PRIORITY_MEDIUM,
	TASK_PRIORITY_LOW,
	TASK_PRIORITY_IDLE
} taskPriority_t;

typedef struct {
	task_t entryPoint;
	taskPriority_t priority;
	uint32_t *stack;
	uint32_t stackPointer;
	uint32_t stackSize;
	taskState_t state;
	uint32_t delay;
	void* initialParameter;
} taskControl_t;

typedef struct {
	uint32_t delay;
	gpioMap_t led;
} blinkTaskData_t;

typedef enum {
   OS_STATE_IRQ = 0,
   OS_STATE_TASK = 1
} osState_t;

typedef struct {
	int32_t             currentTask;
	int32_t             numTask;
	osTicks_t            tickCount;
    taskControl_t       taskList[OS_MAX_TASK + 1];
    taskID_t            readyTaskList[OS_PRIORITY_LEVELS][OS_MAX_TASK+1];
    osState_t           state;
}osControl_t;

void os_start(void);

void set_delay_os(uint32_t ms);

void set_blocked_task_os(taskID_t id);

taskID_t get_current_task(void);

osState_t get_state_os(void);

osTicks_t get_systemTicks_os(void);

void schedule(void);

void os_enter_critical(void);

void add_ready_task_os(taskID_t id);

void os_exit_critical(void);

#endif
