#include "sapi.h"
#include "main.h"
#include "os.h"
#include "os_events.h"

#include "task_button_down.h"

uint8_t stackButtonDown[STACK_SIZE];

extern event_t buttonDownEvent;

extern bool buttonsPressed;

extern osTicks_t downTime;

extern int downButton;

/**
 * Task that detects the events of descending flanks of pressed buttons,
 * an anti-rebound is considered
 * and the falling edge times of both buttons are printed
 */
void taskButtonDown(void *a) {
	int firstButton, secondButton;
	osTicks_t time = 0;

	while (1) {

		/*pulsed first event is expected*/
		os_event_wait(&buttonDownEvent);

		/*if the buttons were not pressed, continue*/
		if (buttonsPressed == false) {

			/*the first button is determined*/
			firstButton = !gpioRead(TEC1) ? TEC1 : TEC2;

			/*the second button is determined*/
			secondButton = firstButton == TEC1 ? TEC2 : TEC1;

			/*the time is set*/
			time = get_systemTicks_os();

			/*anti-rebound first button*/
			set_delay_os(20);

			/*if the first button is still pressed, it continues*/
			if (!gpioRead(firstButton)) {

				/*pulsed second event is expected*/
				os_event_wait(&buttonDownEvent);

				/*anti-rebound first button*/
				set_delay_os(20);

				/*if the second button is still pressed, it continues*/
				if (!gpioRead(secondButton)) {

					/*the buttonsPressed flag is set true*/
					buttonsPressed = true;

					/*t1 time*/
					downTime = get_systemTicks_os() - time;

					/*the first button pressed is set*/
					downButton = firstButton;

					/*the time of the falling edge and the first button is reported*/
					reportTimesTest(firstButton, downTime);
				}
			}
		}
	}
}
