/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TEMP0_ADC_Pin GPIO_PIN_0
#define TEMP0_ADC_GPIO_Port GPIOA
#define TEMP1_ADC_Pin GPIO_PIN_1
#define TEMP1_ADC_GPIO_Port GPIOA
#define TEMP2_ADC_Pin GPIO_PIN_2
#define TEMP2_ADC_GPIO_Port GPIOA
#define TEMP3_ADC_Pin GPIO_PIN_3
#define TEMP3_ADC_GPIO_Port GPIOA
#define HEARTBEAT_Pin GPIO_PIN_6
#define HEARTBEAT_GPIO_Port GPIOA
#define EXTRA_SENSOR_1_Pin GPIO_PIN_4
#define EXTRA_SENSOR_1_GPIO_Port GPIOC
#define EXTRA_SENSOR_2_Pin GPIO_PIN_5
#define EXTRA_SENSOR_2_GPIO_Port GPIOC
#define EXTRA_SENSOR_3_Pin GPIO_PIN_0
#define EXTRA_SENSOR_3_GPIO_Port GPIOB
#define EXTRA_SENSOR_4_Pin GPIO_PIN_1
#define EXTRA_SENSOR_4_GPIO_Port GPIOB
#define TCAN_RX_Pin GPIO_PIN_12
#define TCAN_RX_GPIO_Port GPIOB
#define TCAN_TX_Pin GPIO_PIN_13
#define TCAN_TX_GPIO_Port GPIOB
#define FAN_PWM_Pin GPIO_PIN_8
#define FAN_PWM_GPIO_Port GPIOA
#define PUMP_PWM_Pin GPIO_PIN_9
#define PUMP_PWM_GPIO_Port GPIOA
#define EXTRA_PWM_Pin GPIO_PIN_10
#define EXTRA_PWM_GPIO_Port GPIOA
#define USB_UART_TX_Pin GPIO_PIN_6
#define USB_UART_TX_GPIO_Port GPIOB
#define USB_UART_RX_Pin GPIO_PIN_7
#define USB_UART_RX_GPIO_Port GPIOB
#define PCAN_RX_Pin GPIO_PIN_8
#define PCAN_RX_GPIO_Port GPIOB
#define PCAN_TX_Pin GPIO_PIN_9
#define PCAN_TX_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
