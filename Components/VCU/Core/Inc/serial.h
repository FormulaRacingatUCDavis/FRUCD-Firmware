/*
 * serial.h
 *
 *  Created on: Mar 3, 2024
 *      Author: leoja
 */

#ifndef INC_SERIAL_H_
#define INC_SERIAL_H_

#include "stm32f7xx_hal.h"
#include "stdint.h"

typedef struct
{
	UART_HandleTypeDef* h_uart;
	uint8_t* buf;
	uint32_t len;
	uint32_t index;
} Serial_t;

void Serial_Init(Serial_t* serial, UART_HandleTypeDef* h_uart, uint8_t* buf, uint8_t len);
HAL_StatusTypeDef Serial_StartListening(Serial_t* serial);
uint32_t Serial_BytesAvailable(Serial_t* serial);
uint8_t Serial_GetByte(Serial_t* serial);
HAL_StatusTypeDef Serial_SendBytes(Serial_t* serial, const uint8_t* bytes, uint16_t len, uint32_t timeout_ms);


#endif /* INC_SERIAL_H_ */
