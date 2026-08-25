/*
 * adc.c
 *
 *  Created on: Apr 13, 2024
 *      Author: leoja
 */

#include "adc.h"

void ADC_Input_Init(ADC_Input_t* adc, ADC_HandleTypeDef* h_adc, uint32_t channel, uint32_t rank, uint32_t sample_time)
{
	adc->h_adc = h_adc;
	adc->sConfig.Channel = channel;
	adc->sConfig.Rank = rank;
	adc->sConfig.SamplingTime = sample_time;
	adc->value = 0;
}

HAL_StatusTypeDef ADC_Measure(ADC_Input_t* adc, uint32_t timeout)
{
	HAL_StatusTypeDef hal_res = HAL_OK;

	// select ADC channel
	hal_res = HAL_ADC_ConfigChannel(adc->h_adc, &adc->sConfig);
	if(hal_res != HAL_OK) return hal_res;

	// begin conversion
	hal_res = HAL_ADC_Start(adc->h_adc);
	if(hal_res != HAL_OK) return hal_res;

	// wait for conversion to complete
	hal_res = HAL_ADC_PollForConversion(adc->h_adc, timeout);
	if(hal_res != HAL_OK) return hal_res;

	adc->value = HAL_ADC_GetValue(adc->h_adc);
	return HAL_OK;
}
