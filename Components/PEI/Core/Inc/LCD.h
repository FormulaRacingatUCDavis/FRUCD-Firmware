/*
 * LCD.h
 *
 *  Created on: Aug 23, 2024
 *      Author: Abhineet
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include <stdint.h>
#include "stm32f7xx_hal.h"

void LCD_SetBacklight(uint8_t backlight_state);

void LCD_Init(TIM_HandleTypeDef* const htim_ptr);
void LCD_ReturnHome(TIM_HandleTypeDef* const htim_ptr);
void LCD_Position(TIM_HandleTypeDef* const htim_ptr, uint8_t row_idx, uint8_t col_idx);
void LCD_PrintString(TIM_HandleTypeDef* const htim_ptr, char* const str);

#endif /* INC_LCD_H_ */
