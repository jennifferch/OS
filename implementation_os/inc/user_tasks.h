#ifndef USER_TASKS_H
#define USER_TASKS_H

#include "os_task.h"

#include "task1.h"
#include "task2.h"
#include "task3.h"
#include "task4.h"

#define USER_TASKS                                                      \
       OS_INIT_TASK(task1, (uint32_t *)stack1, STACK_SIZE, 0)           \
       OS_INIT_TASK(task2, (uint32_t *)stack2, STACK_SIZE, 0)           \
       OS_INIT_TASK(task3, (uint32_t *)stack3, STACK_SIZE, 0)           \
       OS_INIT_TASK(task4_button, (uint32_t *)stack4, STACK_SIZE, 0)
#endif
