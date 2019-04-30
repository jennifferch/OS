/*==================[inclusions]=============================================*/

#include "main.h"
#include "sapi.h"
#include "os.h"
#include "os_irq.h"
#include "uart.h"

#include "task_button_down.h"
#include "task_button_up.h"
#include "task_led_green.h"
#include "task_led_red.h"
#include "task_led_yellow.h"
#include "task_led_blue.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/
event_t buttonDownEvent, buttonUpEvent, ledGreenEvent, ledRedEvent,
		ledYellowEvent, ledBlueEvent;
bool buttonsPressed = false;
osTicks_t downTime = 0, upTime = 0;
/*==================[internal functions definition]==========================*/

/**
 *Function of hardware initialization
 */
static void initHardware(void) {
	boardConfig();

	ciaaUARTInit();

	/* External Interrupt for CH0 */
	Chip_SCU_GPIOIntPinSel(CH0_PININT, CH0_GPIO_PORT, CH0_GPIO_PIN);
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(CH0_PININT));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(CH0_PININT));
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(CH0_PININT));
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(CH0_PININT));

	/* External Interrupt for CH1 */
	Chip_SCU_GPIOIntPinSel(CH1_PININT, CH1_GPIO_PORT, CH1_GPIO_PIN);
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(CH1_PININT));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(CH1_PININT));
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(CH1_PININT));
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(CH1_PININT));
}

/*
 *Function that depending on the flank that produced it,
 * releases the corresponding event.
 */
static void gpio_irqHandler(int intChannel) {

	if (Chip_PININT_GetFallStates(LPC_GPIO_PIN_INT) & PININTCH(intChannel)) {
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(intChannel));
		os_event_set(&buttonDownEvent);
	} else {
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(intChannel));
		os_event_set(&buttonUpEvent);
	}
}

/**
 * Function called with interruptions of channel 0 of GPIO
 */
void buttonGPIO0IRQHandler(void) {

	gpio_irqHandler(CH0_PININT);
}

/**
 * Function called with interruptions of channel 1 of GPIO
 */
void buttonGPIO1IRQHandler(void) {

	gpio_irqHandler(CH1_PININT);
}

/**
 * Report the times between the button events
 * and print with the format required by UART USB.
 */
void reportTimes(int led, osTicks_t timeTotal, osTicks_t flancoDesc,
		osTicks_t flancoAsc) {

	dbgPrint("Led ");

	switch (led) {

	case LED1:

		dbgPrint("amarillo ");
		break;
	case LED2:

		dbgPrint("rojo ");
		break;
	case LED3:

		dbgPrint("verde ");
		break;
	case LEDB:

		dbgPrint("azul ");
		break;
	default:

		dbgPrint("verde ");
		break;
	}

	dbgPrint("encendido\r\n");
	dbgPrint("Tiempo encendido ");
	dbgPrint(intToString(timeTotal));
	dbgPrint("ms\r\n");
	dbgPrint("Tiempo entre flancos descendentes ");
	dbgPrint(intToString(flancoDesc));
	dbgPrint("ms\r\n");
	dbgPrint("Tiempo entre flancos ascendentes ");
	dbgPrint(intToString(flancoAsc));
	dbgPrint("ms\r\n");
}

/**
 *The first button pressed or released is reported next to
 *the the total time of the falling edge or rising edge
 */
void reportTimesTest(int firstButton, osTicks_t time) {
	dbgPrint("{");

	if (firstButton == TEC1) {
		dbgPrint("0:1:");
	} else {
		dbgPrint("1:0:");
	}

	dbgPrint(intToString(time));
	dbgPrint("}\r\n");
}

/*==================[external functions definition]==========================*/

int main(void) {
	initHardware();

	// Functions are recorded for GPIO interrupts
	os_AttachIRQ(PIN_INT0_IRQn, buttonGPIO0IRQHandler);
	os_AttachIRQ(PIN_INT1_IRQn, buttonGPIO1IRQHandler);

	// Events are initialized
	os_event_init(&buttonDownEvent); // Pressed button
	os_event_init(&buttonUpEvent); // Released button
	os_event_init(&ledGreenEvent); // Turn on green led
	os_event_init(&ledRedEvent); // Turn on red led
	os_event_init(&ledYellowEvent); // Turn on yellow led
	os_event_init(&ledBlueEvent); // Turn on blue led

	/* Start the os */
	os_start();

	while (1) {
	}
}

#define USER_TASKS                                                           \
	   OS_INIT_TASK(taskLedGreen, TASK_PRIORITY_HIGH, (uint32_t *)stackLedGreen, STACK_SIZE, 0)     \
       OS_INIT_TASK(taskLedRed, TASK_PRIORITY_HIGH, (uint32_t *)stackLedRed, STACK_SIZE, 0)     \
       OS_INIT_TASK(taskLedYellow, TASK_PRIORITY_HIGH, (uint32_t *)stackLedYellow, STACK_SIZE, 0)     \
       OS_INIT_TASK(taskLedBlue, TASK_PRIORITY_HIGH, (uint32_t *)stackLedBlue, STACK_SIZE, 0)     \
       OS_INIT_TASK(taskButtonDown, TASK_PRIORITY_HIGH, (uint32_t *)stackButtonDown, STACK_SIZE, 0)  \
       OS_INIT_TASK(taskButtonUp, TASK_PRIORITY_HIGH, (uint32_t *)stackButtonUp, STACK_SIZE, 0)

taskControl_t tasks_list[] = {
USER_TASKS LAST_TASK };

/*==================[end of file]============================================*/
