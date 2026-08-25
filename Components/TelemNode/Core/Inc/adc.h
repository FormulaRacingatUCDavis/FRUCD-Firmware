/*
 * adc.h
 *
 *  Created on: Apr 13, 2024
 *      Author: leoja
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "stm32f1xx_hal.h"
#include "stdint.h"

typedef struct
{
	ADC_HandleTypeDef* h_adc;
	ADC_ChannelConfTypeDef sConfig;
	uint16_t value;
} ADC_Input_t;

void ADC_Input_Init(ADC_Input_t* adc, ADC_HandleTypeDef* h_adc, uint32_t channel, uint32_t rank, uint32_t sample_time);
HAL_StatusTypeDef ADC_Measure(ADC_Input_t* adc, uint32_t timeout);

#endif /* INC_ADC_H_ */
