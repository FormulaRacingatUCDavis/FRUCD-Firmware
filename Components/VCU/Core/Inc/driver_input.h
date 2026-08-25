/*
 * driver_input.h
 *
 *  Created on: May 17, 2024
 *      Author: Melody
 */

#ifndef INC_DRIVER_INPUT_H_
#define INC_DRIVER_INPUT_H_

#include <stdint.h>

#define BUTTON_DELAY 300
#define SYSTICK_MAX_VAL 16777215

typedef enum {
	NO_BUTTON,
	TC_BUTTON,
	DEBUG_BUTTON,
	MARKER_BUTTON,
	OVERTAKE_BUTTON,
} button_id_t;


typedef enum {
	DISPLAY_DRIVE = 0b00,
	DISPLAY_DEBUG = 0b01,
	DISPLAY_PRACTICE = 0b10
} dashboard_display_state;

void driver_input_update();
uint8_t is_button_enabled(button_id_t button_id);
button_id_t which_button_pressed();


/************ Switches ************/

typedef enum {
	HV_SWITCH,
	DRIVE_SWITCH
} switch_id_t;

uint8_t is_switch_on(switch_id_t switch_id);

#endif /* INC_DRIVER_INPUT_H_ */
