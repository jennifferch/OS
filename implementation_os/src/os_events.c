#include "os.h"
#include "os_events.h"
#include "chip.h"

extern osState_t osState;
extern bool osSwitchRequired;

/**
 * Initializes an event for it's future use.
 */
bool os_event_init(event_t *event) {

	static eventID_t id = 0;

	/* Start the structure of the event.*/
	event->state = EVENT_INITIALIZED;
	event->eventId = id;
	event->taskId = OS_ACTUAL_TASK_NONE;

	/* Increment event id for next one */
	id++;

	return true;
}

/**
 * Waits for an event to be sent. The task that calls this function
 * will be blocked until the event is sent.
 */
bool os_event_wait(event_t *event) {

	// Check if valid calling context
	if (osState == OS_STATE_IRQ) {
		return false;
	}

	/* Disable interrupts.*/
	os_enter_critical();

	/* Check if state of event had been initialized */
	if (event->state != EVENT_INITIALIZED) {
		/* Enable interrupts*/
		os_exit_critical();

		return false;
	}

	/* Get current task */
	taskID_t currentTask = get_current_task();

	/* Save the current task in the event so that it can be unlocked later*/
	event->taskId = currentTask;

	/* Set event as pending */
	event->state = EVENT_WAITING;

	/* Set current task as blocked */
	set_blocked_task_os(currentTask);

	/* Enable interrupts. */
	os_exit_critical();

	/* Call the scheduler for context switching */
	schedule();

	return true;
}

/**
 * Sends an event to unblock to task that was waiting for it.
 */
bool os_event_set(event_t *event) {

	/* Disable interrupts */
	os_enter_critical();

	if (osState == OS_STATE_IRQ) {
		// Mark that a scheduler update is needed
		osSwitchRequired = true;
	}

	/* Check if state of the event was pending */
	if (event->state != EVENT_WAITING) {
		/* Enable interrupts */
		os_exit_critical();

		return false;
	}

	/* Add the blocked task as ready */
	add_ready_task_os(event->taskId);

	/* Clear the event's task id */
	event->taskId = OS_ACTUAL_TASK_NONE;

	/* Reset the event's state */
	event->state = EVENT_INITIALIZED;

	/* Enable interrupts */
	os_exit_critical();

	// Call switch context only if not calling from an IRQ
	if (osState != OS_STATE_IRQ) {
		// Call the scheduler for context switching
		schedule();
	}

	return true;
}
