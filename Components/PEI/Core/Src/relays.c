/*
 * relays.c
 *
 *  Created on: Aug 23, 2024
 *      Author: Abhineet
 */


#include <stdint.h>
#include "stm32f7xx_hal.h"

#include "relays.h"
#include "main.h"

uint8_t relay_flags = 0b000;

// Closes the precharge relay, opens the positive relay
void start_precharge() {
	HAL_GPIO_WritePin(AIR_Neg_GPIO_Port, AIR_Neg_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(AIR_Pos_GPIO_Port, AIR_Pos_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Precharge_GPIO_Port, Precharge_Pin, GPIO_PIN_SET);

	relay_flags = 0b101;
}

// Opens the precharge relay, closes the positive relay
void finish_precharge() {
	HAL_GPIO_WritePin(AIR_Neg_GPIO_Port, AIR_Neg_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(AIR_Pos_GPIO_Port, AIR_Pos_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(Precharge_GPIO_Port, Precharge_Pin, GPIO_PIN_RESET);

	relay_flags = 0b110;
}

void clear_interlock() {
	HAL_GPIO_WritePin(AIR_Neg_GPIO_Port, AIR_Neg_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(AIR_Pos_GPIO_Port, AIR_Pos_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Precharge_GPIO_Port, Precharge_Pin, GPIO_PIN_RESET);

	relay_flags = 0b000;
}
