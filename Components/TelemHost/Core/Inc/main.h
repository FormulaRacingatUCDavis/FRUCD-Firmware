/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#define MON_12V_Pin GPIO_PIN_8
#define MON_12V_GPIO_Port GPIOF
#define GPIO1_3V3_Pin GPIO_PIN_9
#define GPIO1_3V3_GPIO_Port GPIOF
#define GPIO2_3v3_Pin GPIO_PIN_10
#define GPIO2_3v3_GPIO_Port GPIOF
#define ETH_INT_Pin GPIO_PIN_0
#define ETH_INT_GPIO_Port GPIOC
#define HEARTBEAT_Pin GPIO_PIN_4
#define HEARTBEAT_GPIO_Port GPIOA
#define USB_UART_TX_Pin GPIO_PIN_8
#define USB_UART_TX_GPIO_Port GPIOD
#define USB_UART_RX_Pin GPIO_PIN_9
#define USB_UART_RX_GPIO_Port GPIOD
#define TCAN_RX_Pin GPIO_PIN_11
#define TCAN_RX_GPIO_Port GPIOA
#define TCAN_TX_Pin GPIO_PIN_12
#define TCAN_TX_GPIO_Port GPIOA
#define XSENS_UART_TX_Pin GPIO_PIN_5
#define XSENS_UART_TX_GPIO_Port GPIOD
#define XSENS_UART_RX_Pin GPIO_PIN_6
#define XSENS_UART_RX_GPIO_Port GPIOD
#define Card_Detect_Pin GPIO_PIN_9
#define Card_Detect_GPIO_Port GPIOG
#define GASP_INTERRUPT_Pin GPIO_PIN_15
#define GASP_INTERRUPT_GPIO_Port GPIOG
#define GASP_INTERRUPT_EXTI_IRQn EXTI15_10_IRQn
#define PCAN_RX_Pin GPIO_PIN_5
#define PCAN_RX_GPIO_Port GPIOB
#define PCAN_TX_Pin GPIO_PIN_6
#define PCAN_TX_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
