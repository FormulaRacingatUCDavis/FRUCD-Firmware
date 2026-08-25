/*
 * driver_input.c
 *
 *  Created on: Jan 6, 2025
 *      Author: Melody
 */
#include <stdio.h>
#include "main.h"
#include "driver_input.h"
#include "stm32f7xx_hal.h"
#include "can_manager.h"

#define NUM_BUTTONS 4

uint8_t is_overriding_cooling = 0;

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

typedef struct {
	volatile uint32_t last_valid_pressed_time;
	volatile uint8_t enabled;
//	void (*onEnable)();
} button_state_t;

button_state_t button_states[NUM_BUTTONS] = {
	{last_valid_pressed_time: 0, enabled: 0}, // TC
	{last_valid_pressed_time: 0, enabled: 0}, // debug
	{last_valid_pressed_time: 0, enabled: 0}, // marker
	{last_valid_pressed_time: 0, enabled: 0} // overtake
};

button_id_t which_button_pressed() { // TODO Can change these pins depending on electrical
	if (!HAL_GPIO_ReadPin(GPIOG, BUTTON1_Pin)) {
		return TC_BUTTON;
	}
	if (!HAL_GPIO_ReadPin(GPIOG, BUTTON2_Pin)) {
		return DEBUG_BUTTON;
	}
	if (!HAL_GPIO_ReadPin(GPIOG, BUTTON3_Pin)) {
		return MARKER_BUTTON;
	}
	if (!HAL_GPIO_ReadPin(GPIOG, BUTTON4_Pin)) {
		return OVERTAKE_BUTTON;
	}
	return NO_BUTTON;

}

uint8_t is_button_enabled(button_id_t button_id) {
	return button_states[button_id].enabled;
}

uint8_t marker_data[8] = {0,0,0,0,0,0,0,0};

// runs once only when initially enabled
void on_button_enabled(button_id_t enabled_id) {
	switch (enabled_id) {
		case DEBUG_BUTTON:
			if (dashboard_display_mode == DISPLAY_DRIVE) {
				dashboard_display_mode = DISPLAY_DEBUG;
			} else if (dashboard_display_mode == DISPLAY_DEBUG) {
				dashboard_display_mode = DISPLAY_PRACTICE;
			} else if (dashboard_display_mode == DISPLAY_PRACTICE) {
				dashboard_display_mode = DISPLAY_DRIVE;
			} else {
				dashboard_display_mode = DISPLAY_DRIVE; // just in case, default to drive display mode
			}

			can_tx_buttons_and_knobs(&hcan1);
			break;
		case TC_BUTTON:
			can_tx_buttons_and_knobs(&hcan1);
			break;
		case MARKER_BUTTON:
			can_tx_buttons_and_knobs(&hcan1);
			break;
		case OVERTAKE_BUTTON:
			can_tx_buttons_and_knobs(&hcan1);

			// if in debug mode, use overtake button for cooling loop override
			if (is_button_enabled(DEBUG_BUTTON)) {
				if (is_overriding_cooling == 1) { // if already overriding, stop overriding
					is_overriding_cooling = 0;
				} else { // else, start overriding
					is_overriding_cooling = 1;
				}
//				can_tx_override_cooling_request(&hcan2, is_overriding_cooling);
			}

			break;
		default:

	}
}

// runs once when disabled
void on_button_disabled(button_id_t disabled_id) {
	switch (disabled_id) {
		case DEBUG_BUTTON:
			can_tx_buttons_and_knobs(&hcan1);
			break;
		case TC_BUTTON:
			can_tx_buttons_and_knobs(&hcan1);
			break;
		case MARKER_BUTTON:
			can_tx_buttons_and_knobs(&hcan1);
			break;
		case OVERTAKE_BUTTON:
			can_tx_buttons_and_knobs(&hcan1);
			break;
		default:
	}
}

void driver_input_update() {
	button_id_t pressed_btn_id = which_button_pressed();
	if (pressed_btn_id != NO_BUTTON) {
		int32_t time_diff = HAL_GetTick() - button_states[pressed_btn_id].last_valid_pressed_time;
		if (time_diff < 0) {
			// timer overflowed and wrapped around
			time_diff = SYSTICK_MAX_VAL + time_diff;
		}

		// enable feature after button stays "pressed" long enough, avoids oscillating presses
		// less of an issue if electrical implemented hardware button debouncing :(
		if (time_diff > BUTTON_DELAY) {
			button_states[pressed_btn_id].last_valid_pressed_time = HAL_GetTick();

			if (button_states[pressed_btn_id].enabled) {
				button_states[pressed_btn_id].enabled = 0;
				on_button_disabled(pressed_btn_id);

			}
			else {
				button_states[pressed_btn_id].enabled = 1;
				on_button_enabled(pressed_btn_id);
			}
		}
	}
}

uint8_t is_switch_on(switch_id_t switch_id) {
	if (switch_id == HV_SWITCH) { return !HAL_GPIO_ReadPin(GPIOG, HV_REQUEST_Pin); }
	if (switch_id == DRIVE_SWITCH) { return !HAL_GPIO_ReadPin(GPIOG, DRIVE_REQUEST_Pin); }
	return 0;
}

//uint8_t hv_switch() {
//	return !HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_13);
//}
//
//uint8_t drive_switch() {
//	return !HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_14);
//}

