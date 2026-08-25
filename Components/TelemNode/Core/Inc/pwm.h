/*
 * pwm.h
 *
 *  Created on: Apr 15, 2024
 *      Author: leoja
 */

#ifndef INC_PWM_H_
#define INC_PWM_H_

#include "stm32f1xx_hal.h"
#include "stdint.h"

typedef struct
{
	TIM_HandleTypeDef* h_tim;
	uint32_t channel;
} PWM_Output_t;

HAL_StatusTypeDef PWM_Init(PWM_Output_t* pwm, TIM_HandleTypeDef* h_tim, uint32_t channel);
void PWM_SetDutyCycle(PWM_Output_t* pwm, uint8_t duty);

#endif /* INC_PWM_H_ */
