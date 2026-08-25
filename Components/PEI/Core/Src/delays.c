/*
 * delays.c
 *
 *  Created on: Aug 23, 2024
 *      Author: Abhineet
 */


#include "delays.h"

// Delay in microseconds
void delay_us(TIM_HandleTypeDef* const htim_ptr, uint16_t us) {
	uint16_t count = us * 8; // number of timer counts needed assuming each timer count is 125 ns

	__HAL_TIM_SET_COUNTER(htim_ptr, 0);
	while (__HAL_TIM_GET_COUNTER(htim_ptr) < count);
}

// Delay 500 nanoseconds (0.5 us)
void delay_500_ns(TIM_HandleTypeDef* const htim_ptr) {
	__HAL_TIM_SET_COUNTER(htim_ptr, 0);
	while (__HAL_TIM_GET_COUNTER(htim_ptr) < 4);
}

// Delay 125 nanoseconds
void delay_125_ns(TIM_HandleTypeDef* const htim_ptr) {
	__HAL_TIM_SET_COUNTER(htim_ptr, 0);
	while (__HAL_TIM_GET_COUNTER(htim_ptr) < 1); // Each count of the timer should be 125 ns long
}
