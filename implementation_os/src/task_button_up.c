#include "sapi.h"
#include "main.h"
#include "os.h"
#include "os_events.h"

#include "task_button_up.h"

uint8_t stackButtonUp[STACK_SIZE];

extern event_t buttonUpEvent, ledGreenEvent, ledRedEvent, ledYellowEvent,
		ledBlueEvent;

extern bool buttonsPressed;

extern osTicks_t downTime;

osTicks_t tTotal;

osTicks_t upTime;

int downButton;

int upButton;

/**
 * Task that detects the events of ascending flanks of pressed buttons,
 * an anti-rebound is considered
 * and the rising edge times of both buttons are printed
 */
void taskButtonUp(void *a) {
	int firstButton, secondButton;
	osTicks_t time = 0;

	while (1) {

		/*released first event is expected*/
		os_event_wait(&buttonUpEvent);

		/*if the buttons are pressed, continue*/
		if (buttonsPressed == true) {

			/*the first button is determined*/
			firstButton = gpioRead(TEC1) ? TEC1 : TEC2;

			/*the second button is determined*/
			secondButton = firstButton == TEC1 ? TEC2 : TEC1;

			/*the time is set*/
			time = get_systemTicks_os();

			/*anti-rebound first button*/
			set_delay_os(20);

			/*if the button is still released it continues*/
			if (gpioRead(firstButton)) {

				/*released second event is expected*/
				os_event_wait(&buttonUpEvent);

				/*anti-rebound second button*/
				set_delay_os(20);

				/* if the second button is still released, the time is reported */
				if (gpioRead(secondButton)) {

					/*the buttonsPressed flag is set false*/
					buttonsPressed = false;

					/*t2 time*/
					upTime = get_systemTicks_os() - time;

					/*the time of the released edge and the first button is reported*/
					reportTimesTest(firstButton, upTime);

					/*t2 time*/
					tTotal = downTime + upTime;

					/*the first button released is set*/
					upButton = firstButton;

					/*if TEC1 is the first pressed and the first released, then the green LED is on*/
					if (downButton == TEC1 && upButton == TEC1) {

						reportTimes(LED3, tTotal, downTime, upTime);
						os_event_set(&ledGreenEvent);
						/*if TEC1 is the first pressed and the TEC2 is the first released, then the red LED is on*/
					} else if (downButton == TEC1 && upButton == TEC2) {

						reportTimes(LED2, tTotal, downTime, upTime);
						os_event_set(&ledRedEvent);
						/*if TEC2 is the first pressed and the TEC1 is the first released, then the yellow LED is on*/
					} else if (downButton == TEC2 && upButton == TEC1) {

						reportTimes(LED1, tTotal, downTime, upTime);
						os_event_set(&ledYellowEvent);
						/*if TEC2 is the first pressed and the first released, then the blue LED is on*/
					} else if (downButton == TEC2 && upButton == TEC2) {

						reportTimes(LEDB, tTotal, downTime, upTime);
						os_event_set(&ledBlueEvent);
					}
				}
			}
		}
	}
}
