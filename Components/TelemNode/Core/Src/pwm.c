/*
 * pwm.c
 *
 *  Created on: Apr 15, 2024
 *      Author: leoja
 */

#include "pwm.h"

HAL_StatusTypeDef PWM_Init(PWM_Output_t* pwm, TIM_HandleTypeDef* h_tim, uint32_t channel)
{
	pwm->h_tim = h_tim;
	pwm->channel = channel;

	return HAL_TIM_PWM_Start(h_tim, channel);
}

void PWM_SetDutyCycle(PWM_Output_t* pwm, uint8_t duty)
{
	uint32_t ccr = pwm->h_tim->Instance->ARR * (255-duty) / 255;

	switch(pwm->channel)
	{
		case TIM_CHANNEL_1:
			pwm->h_tim->Instance->CCR1 = ccr;
			break;
		case TIM_CHANNEL_2:
			pwm->h_tim->Instance->CCR2 = ccr;
			break;
		case TIM_CHANNEL_3:
			pwm->h_tim->Instance->CCR3 = ccr;
			break;
		case TIM_CHANNEL_4:
			pwm->h_tim->Instance->CCR4 = ccr;
			break;
		default:
			//YO WTF?
	}
}
