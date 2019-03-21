#ifndef OS_TASK_H
#define OS_TASK_H

#include "stdint.h"

#define LAST_OS_TASK {0,0,0,0,0}

/***********************************************************************/
#define STACK_SIZE  512

#define INITIAL_xPSR    (1 << 24)

#define EXC_RETURN (0xFFFFFFF9)

#define ACTUAL_TASK_MAIN -1


/***********************************************************************/
typedef void(*task_t)(void *);

typedef enum {
   TASK_ERROR,
   TASK_READY,
   TASK_RUNNING,
   TASK_BLOCKED,
   TASK_HALTED,
   TASK_TERMINATED
} taskState_t;

typedef struct {
   task_t entryPoint;
   uint32_t *stack;
   uint32_t stackPointer;
   uint32_t stackSize;
   taskState_t state;
   void* initialParameter;
} taskControl_t;

#endif
