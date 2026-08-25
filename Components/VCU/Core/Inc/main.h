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
#include "stm32f7xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
uint8_t shutdown_closed();
uint8_t traction_control_enable();
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BUZZER_Pin GPIO_PIN_2
#define BUZZER_GPIO_Port GPIOF
#define BSE_R_Pin GPIO_PIN_3
#define BSE_R_GPIO_Port GPIOF
#define CURRENT_DIGITAL_Pin GPIO_PIN_4
#define CURRENT_DIGITAL_GPIO_Port GPIOF
#define BSE_F_Pin GPIO_PIN_5
#define BSE_F_GPIO_Port GPIOF
#define HARD_BSPD_Pin GPIO_PIN_6
#define HARD_BSPD_GPIO_Port GPIOF
#define LV_BAT_MEASURE_Pin GPIO_PIN_7
#define LV_BAT_MEASURE_GPIO_Port GPIOF
#define EXTRA_SENS2_Pin GPIO_PIN_9
#define EXTRA_SENS2_GPIO_Port GPIOF
#define APP2_Pin GPIO_PIN_10
#define APP2_GPIO_Port GPIOF
#define APP1_Pin GPIO_PIN_0
#define APP1_GPIO_Port GPIOC
#define EXTRA_SENS1_Pin GPIO_PIN_1
#define EXTRA_SENS1_GPIO_Port GPIOC
#define KNOB2_Pin GPIO_PIN_2
#define KNOB2_GPIO_Port GPIOC
#define KNOB1_Pin GPIO_PIN_3
#define KNOB1_GPIO_Port GPIOC
#define HEARTBEAT_Pin GPIO_PIN_3
#define HEARTBEAT_GPIO_Port GPIOA
#define TCAN_RX_Pin GPIO_PIN_12
#define TCAN_RX_GPIO_Port GPIOB
#define TCAN_TX_Pin GPIO_PIN_13
#define TCAN_TX_GPIO_Port GPIOB
#define USB_UART_TX_Pin GPIO_PIN_8
#define USB_UART_TX_GPIO_Port GPIOD
#define USB_UART_RX_Pin GPIO_PIN_9
#define USB_UART_RX_GPIO_Port GPIOD
#define PCAN_RX_Pin GPIO_PIN_11
#define PCAN_RX_GPIO_Port GPIOA
#define PCAN_TX_Pin GPIO_PIN_12
#define PCAN_TX_GPIO_Port GPIOA
#define BUTTON4_Pin GPIO_PIN_9
#define BUTTON4_GPIO_Port GPIOG
#define BUTTON3_Pin GPIO_PIN_10
#define BUTTON3_GPIO_Port GPIOG
#define BUTTON2_Pin GPIO_PIN_11
#define BUTTON2_GPIO_Port GPIOG
#define BUTTON1_Pin GPIO_PIN_12
#define BUTTON1_GPIO_Port GPIOG
#define HV_REQUEST_Pin GPIO_PIN_13
#define HV_REQUEST_GPIO_Port GPIOG
#define DRIVE_REQUEST_Pin GPIO_PIN_14
#define DRIVE_REQUEST_GPIO_Port GPIOG

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
