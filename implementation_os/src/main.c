/*==================[inclusions]=============================================*/

#include "main.h"
#include "sapi.h"
#include "os.h"
#include "os_task.h"

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

   start_os();

   while (1) {}
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
