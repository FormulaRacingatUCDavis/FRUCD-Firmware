#ifndef TELEM_NODE_H
#define TELEM_NODE_H


#include "main.h"
#include "config.h"

typedef struct {
	uint32_t channel_number;
	uint8_t flag;
	uint16_t adc_value;
} ADC_Channel_t;

#define TEMP0_CHANNEL ADC_CHANNEL_0
#define TEMP1_CHANNEL ADC_CHANNEL_1
#define TEMP2_CHANNEL ADC_CHANNEL_2
#define TEMP3_CHANNEL ADC_CHANNEL_3

#define NUM_ADC_CHANNELS 4
extern volatile ADC_Channel_t adc_channels[NUM_ADC_CHANNELS];


// PUBLIC FUNCTION PROTOTYPES
void Cooling_Init();
void Cooling_Update();
void Sensor_Update(CAN_HandleTypeDef *hcan);
void CAN_Send_Temp_ADC(CAN_HandleTypeDef *hcan);

#endif
