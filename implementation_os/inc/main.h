#ifndef _MAIN_H_
#define _MAIN_H_

/*==================[inclusions]=============================================*/
#include "os.h"
/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

#define CH0_PININT 0
#define CH1_PININT 1

#define CH0_PIN_INT_IRQn PIN_INT0_IRQn
#define CH1_PIN_INT_IRQn PIN_INT1_IRQn

#define CH0_GPIO_PORT   0
#define CH0_GPIO_PIN    4
#define CH1_GPIO_PORT   0
#define CH1_GPIO_PIN    8

#define CH0_IRQ_HANDLER GPIO0_IRQHandler
#define CH1_IRQ_HANDLER GPIO1_IRQHandler

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

int main(void);

void buttonGPIO0IRQHandler(void);

void buttonGPIO1IRQHandler(void);

void reportTimes(int led, osTicks_t timeTotal, osTicks_t flancoDesc, osTicks_t flancoAsc ) ;

void reportTimesTest(int firstButton, osTicks_t time);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif /* #ifndef _MAIN_H_ */
