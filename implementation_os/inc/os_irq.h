#ifndef _OS_IRQ_H_
#define _OS_IRQ_H_

/*==================[inclusions]=============================================*/

#include "board.h"

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/** handler de irq of user */
typedef void (*user_irq_handler_t)(void);

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

int32_t os_AttachIRQ(LPC43XX_IRQn_Type irq, user_irq_handler_t callback);
int32_t os_DetachIRQ(LPC43XX_IRQn_Type irq);

/*==================[end of file]============================================*/
#endif /* #ifndef _OS_IRQ_H_ */
