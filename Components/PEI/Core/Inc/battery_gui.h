/*
 * battery_gui.h
 *
 *  Created on: Dec 6, 2024
 *      Author: Abhineet
 */

#ifndef INC_BATTERY_GUI_H_
#define INC_BATTERY_GUI_H_

#include <stdint.h>
#include "stm32f7xx_hal.h"

void uart_send_GUI_Data(UART_HandleTypeDef* const huart);
uint8_t uart_receive_charge_profile(UART_HandleTypeDef* const huart);

#endif /* INC_BATTERY_GUI_H_ */
