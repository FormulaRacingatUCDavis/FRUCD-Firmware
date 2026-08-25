/*
 * display.h
 *
 *  Created on: Oct 24, 2024
 *      Author: Abhineet
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "stm32f7xx_hal.h"

void update_display(TIM_HandleTypeDef* const htim_ptr);

#endif /* INC_DISPLAY_H_ */
