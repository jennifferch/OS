#ifndef OS_H
#define OS_H

#include "stdint.h"
#include "sapi.h"

#define LAST_OS_TASK {0,0,0,0,0}

/***********************************************************************/
#define STACK_SIZE  512

#define INITIAL_xPSR    (1 << 24)

#define EXC_RETURN (0xFFFFFFF9)

#define ACTUAL_TASK_MAIN -1

#define OS_ACTUAL_TASK_NONE -1

#define OS_MAX_TASKS  8

#define OS_INIT_TASK(_entryPoint, _stack, _stackSize, _parameter)        \
      {.entryPoint = _entryPoint, .stack = _stack, .stackPointer = 0,         \
      .stackSize = _stackSize, .state = TASK_STATE_READY,                           \
      .initialParameter = _parameter, .delay = 0},

/***********************************************************************/
/***********************************************************************/
typedef void (*task_t)(void *);

typedef enum taskState_t {
	TASK_STATE_ERROR,
	TASK_STATE_READY,
	TASK_STATE_RUNNING,
	TASK_STATE_BLOCKED,
	TASK_STATE_TERMINATED
} taskState_t;

typedef struct {
	task_t entryPoint;
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

typedef enum taskPriority_t {
	TASK_PRIORITY_IDLE,
	TASK_PRIORITY_LOW,
	TASK_PRIORITY_MEDIUM,
	TASK_PRIORITY_HIGH,
} taskPriority_t;
/*crear las prioridades listarlas todas y luego usarlas en las tareas es mas seguro si le pongo un puntego largarlas a ver que sucede luego*/

void os_start(void);

void set_delay_os(uint32_t ms);

#endif
