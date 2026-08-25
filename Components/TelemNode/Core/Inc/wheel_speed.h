/*
 * wheel_speed.h
 *
 *  Created on: Apr 17, 2024
 *      Author: leoja
 */

#ifndef INC_WHEEL_SPEED_H_
#define INC_WHEEL_SPEED_H_

#include "stm32f1xx_hal.h"
#include "stdint.h"

typedef struct
{
	uint32_t last_tick;
	uint16_t last_count;
	uint16_t count;
	TIM_HandleTypeDef* tim;
} WheelSpeed_t;

void WheelSpeed_Init(WheelSpeed_t* ws, TIM_HandleTypeDef* tm);
uint32_t WheelSpeed_GetCPS(WheelSpeed_t* ws);

#endif /* INC_WHEEL_SPEED_H_ */
