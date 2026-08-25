/*
 * delays.h
 *
 *  Created on: Aug 23, 2024
 *      Author: Abhineet
 */

#ifndef INC_DELAYS_H_
#define INC_DELAYS_H_

#include <stdint.h>
#include "stm32f7xx_hal.h"

void delay_us(TIM_HandleTypeDef* const htim_ptr, uint16_t us);
void delay_500_ns(TIM_HandleTypeDef* const htim_ptr);
void delay_125_ns(TIM_HandleTypeDef* const htim_ptr);

#endif /* INC_DELAYS_H_ */
