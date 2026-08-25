#ifndef TELEM_NODE_H
#define TELEM_NODE_H


#include "main.h"
#include "config.h"


// PUBLIC FUNCTION PROTOTYPES
void Cooling_Init();
void Cooling_Update();

void CAN_Send_Temp_ADC(CAN_HandleTypeDef *hcan);

#endif
