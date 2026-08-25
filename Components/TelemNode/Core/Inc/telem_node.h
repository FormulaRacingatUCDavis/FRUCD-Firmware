#ifndef TELEM_NODE_H
#define TELEM_NODE_H


#include "main.h"
#include "config.h"


// PUBLIC FUNCTION PROTOTYPES
void TelemNode_Init();
void TelemNode_Update();
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

uint16_t get_strain_gauge_val(uint64_t adc_val);

uint16_t get_shock_angle(uint64_t adc_val);

uint16_t get_ir_brake_temp(uint64_t adc_val);

uint16_t get_k_brake_temp(uint64_t adc_val);

#endif
