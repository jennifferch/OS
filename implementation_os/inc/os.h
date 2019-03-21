#ifndef OS_H
#define OS_H

#include "stdint.h"
#include "user_tasks.h"

#ifndef USER_TASKS
#error "USER_TASKS' macro to define user tasks in os"
#endif

/***********************************************************************/

void start_os(void);

#endif
