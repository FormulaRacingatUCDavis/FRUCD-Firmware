/*
 * LCD.c
 *
 *  Created on: Aug 23, 2024
 *      Author: Abhineet
 */


#include "LCD.h"
#include "main.h"
#include "delays.h"

// Wait until LCD's busy flag is false
static void wait_for_LCD(TIM_HandleTypeDef* const htim_ptr) {
	GPIO_PinState is_busy = GPIO_PIN_SET;
	GPIO_InitTypeDef GPIO_InitStruct;

	// Change DB7 pin mode to input (busy flag is read from DB7)
	HAL_GPIO_DeInit(LCD_DB7_GPIO_Port, LCD_DB7_Pin);
	GPIO_InitStruct.Pin = LCD_DB7_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(LCD_DB7_GPIO_Port, &GPIO_InitStruct);

	do {
		delay_us(htim_ptr, 80);

		HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, GPIO_PIN_SET);
		delay_us(htim_ptr, 80);

		is_busy = HAL_GPIO_ReadPin(LCD_DB7_GPIO_Port, LCD_DB7_Pin);
		HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_RESET);
	} while (is_busy);

	// Change DB7 pin mode to output
	HAL_GPIO_DeInit(LCD_DB7_GPIO_Port, LCD_DB7_Pin);
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LCD_DB7_GPIO_Port, &GPIO_InitStruct);
}

static void send_instruction(TIM_HandleTypeDef* const htim_ptr,
							 GPIO_PinState rs,
							 GPIO_PinState rw,
							 uint8_t instruction,
							 uint8_t check_busy) {

	if (check_busy) wait_for_LCD(htim_ptr);

	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, rs);
	HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, rw);
	HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_SET);

	delay_125_ns(htim_ptr); // wait for enable to rise

	// Send data over data bus pins
	HAL_GPIO_WritePin(LCD_DB0_GPIO_Port, LCD_DB0_Pin, (instruction & 0x01));
	HAL_GPIO_WritePin(LCD_DB1_GPIO_Port, LCD_DB1_Pin, ((instruction >> 1) & 0x01));
	HAL_GPIO_WritePin(LCD_DB2_GPIO_Port, LCD_DB2_Pin, ((instruction >> 2) & 0x01));
	HAL_GPIO_WritePin(LCD_DB3_GPIO_Port, LCD_DB3_Pin, ((instruction >> 3) & 0x01));
	HAL_GPIO_WritePin(LCD_DB4_GPIO_Port, LCD_DB4_Pin, ((instruction >> 4) & 0x01));
	HAL_GPIO_WritePin(LCD_DB5_GPIO_Port, LCD_DB5_Pin, ((instruction >> 5) & 0x01));
	HAL_GPIO_WritePin(LCD_DB6_GPIO_Port, LCD_DB6_Pin, ((instruction >> 6) & 0x01));
	HAL_GPIO_WritePin(LCD_DB7_GPIO_Port, LCD_DB7_Pin, ((instruction >> 7) & 0x01));

	delay_125_ns(htim_ptr); // wait for data setup

	HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_RESET);

	delay_125_ns(htim_ptr); // data hold

	// Reset data bus pins
	HAL_GPIO_WritePin(LCD_DB0_GPIO_Port, LCD_DB0_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DB1_GPIO_Port, LCD_DB1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DB2_GPIO_Port, LCD_DB2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DB3_GPIO_Port, LCD_DB3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DB4_GPIO_Port, LCD_DB4_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DB5_GPIO_Port, LCD_DB5_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DB6_GPIO_Port, LCD_DB6_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DB7_GPIO_Port, LCD_DB7_Pin, GPIO_PIN_RESET);

	// Prepare for next busy flag check
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);
}

void LCD_SetBacklight(uint8_t backlight_state) {
	GPIO_PinState pin_state = backlight_state == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET;
	HAL_GPIO_WritePin(LCD_Backlight_GPIO_Port, LCD_Backlight_Pin, pin_state);
}

void LCD_Init(TIM_HandleTypeDef* const htim_ptr) {
	LCD_SetBacklight(1);
	HAL_Delay(45); // Wait more than 40 ms

	// Enable 8-bit mode, 2-line display
	send_instruction(htim_ptr, GPIO_PIN_RESET, GPIO_PIN_RESET, 0x38, 0);
	delay_us(htim_ptr, 40); // wait for LCD to process instruction, can't check busy flag

	send_instruction(htim_ptr, GPIO_PIN_RESET, GPIO_PIN_RESET, 0x38, 0);

	// Turn display on
	send_instruction(htim_ptr, GPIO_PIN_RESET, GPIO_PIN_RESET, 0x0C, 1);

	// Clear display
	send_instruction(htim_ptr, GPIO_PIN_RESET, GPIO_PIN_RESET, 0x01, 1);

	// Set cursor to move right, disable display shifting
	send_instruction(htim_ptr, GPIO_PIN_RESET, GPIO_PIN_RESET, 0x06, 1);
}

void LCD_ReturnHome(TIM_HandleTypeDef* const htim_ptr) {
	send_instruction(htim_ptr, GPIO_PIN_RESET, GPIO_PIN_RESET, 0x02, 1);
}

void LCD_Position(TIM_HandleTypeDef* const htim_ptr, uint8_t row_idx, uint8_t col_idx) {
	uint8_t instruction = 0x80;
	uint8_t ddram_addr = 0;

	ddram_addr += 0x40 * row_idx;
	ddram_addr += 0x01 * col_idx;
	instruction += ddram_addr;

	send_instruction(htim_ptr, GPIO_PIN_RESET, GPIO_PIN_RESET, instruction, 1);
}

void LCD_PrintString(TIM_HandleTypeDef* const htim_ptr, char* const str) {
	uint8_t index = 0;
	uint8_t character = str[index];

	while (character != '\0') {
		send_instruction(htim_ptr, GPIO_PIN_SET, GPIO_PIN_RESET, character, 1);

		index++;
		character = str[index];
	}
}
