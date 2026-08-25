/*
 * charger.h
 *
 *  Created on: Sep 19, 2024
 *      Author: Abhineet
 */

#ifndef INC_CHARGER_H_
#define INC_CHARGER_H_

#include <stdint.h>

#define CHARGER_MAX_VOLTAGE 5040u // 504 V

#define CHARGE_STOP 1u
#define CHARGE_START 0u

typedef struct {
	uint16_t charger_max_current;
	uint8_t attenuation_threshold;
} Charge_Profile_t;

void update_max_charge_current();

#endif /* INC_CHARGER_H_ */
