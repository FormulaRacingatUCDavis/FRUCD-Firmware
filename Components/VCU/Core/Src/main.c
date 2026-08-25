/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "inttypes.h"
#include "can_manager.h"
#include "sensors.h"
#include "fsm.h"
#include "traction_control.h"
#include "driver_input.h"
#include "serial_print.h"
#include "config.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc3;
DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_adc3;

CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

// Keeps track of timer waiting for pre-charging
volatile unsigned int precharge_timer_ms = 0;
volatile uint8_t init_fault_cleared = 0;
extern uint32_t torque_req;

// for drive buzzer
#define BUZZ_TIME_MS 1500

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_CAN1_Init(void);
static void MX_CAN2_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_ADC3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t shutdown_closed() {
    if (estop_flags) return 0;
    return (shutdown_flags & 0b00111000) == 0b00111000;
}

void buzzerer()
{
	static state_t last_vcu_state = LV;
	static uint32_t buzz_start = 0;
	uint32_t tick = HAL_GetTick();

	if(last_vcu_state == HV_ENABLED && state == DRIVE)
	{
		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, 1); // turn on buzzer
		buzz_start = tick;
	}
	else if(state != DRIVE || (tick - buzz_start) > BUZZ_TIME_MS)
	{
		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, 0); // turn off buzzer
	}
	else
	{
		// ice cream?
	}

	last_vcu_state = state;
}


unsigned int discrepancy_timer_ms = 0;

extern volatile uint32_t torque_percentage;
extern volatile uint32_t launch_control_param;
extern volatile uint32_t prev_torque_percentage;
extern volatile uint32_t prev_launch_control_param;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_CAN1_Init();
  MX_CAN2_Init();
  MX_ADC1_Init();
  MX_USART3_UART_Init();
  MX_ADC3_Init();
  /* USER CODE BEGIN 2 */

	uint32_t precharge_tick_start = 0;
	init_sensors();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	// driver input
	driver_input_update();

	update_sensor_vals(&hadc1, &hadc3);

	buzzerer();

	/*
	 * T.4.2.5 in FSAE 2022 rulebook
	 * If an implausibility occurs between the values of the APPSs and
	 * persists for more than 100 msec, the power to the Motor(s) must
	 * be immediately stopped completely.
	 *
	 * It is not necessary to Open the Shutdown Circuit, the motor
	 * controller(s) stopping the power to the Motor(s) is sufficient.
	 */
	if (has_discrepancy()) {
		discrepancy_timer_ms += TMR1_PERIOD_MS;
		if (discrepancy_timer_ms > MAX_DISCREPANCY_MS && state == DRIVE) {
			report_fault(SENSOR_DISCREPANCY);
		}
	} else {
		discrepancy_timer_ms = 0;
	}

	// Transmit CAN messages
	can_tx_vcu_state(&hcan1);
	can_tx_torque_request(&hcan1);
	can_tx_pedals_raw(&hcan1);
	can_tx_power(&hcan1);


	// traction control // TODO ADD BACK ONCE WHEEL SPEEDS HAVE BEEN FIGURED OUT
//	if (is_button_enabled(TC_BUTTON)) {
//		traction_control_PID(front_right_wheel_speed, front_left_wheel_speed);
//	}

	// If shutdown circuit opens in any state
	if (!shutdown_closed()) {
		report_fault(SHUTDOWN_CIRCUIT_OPEN);
	}

	//if hard BSPD trips in any state
//	if (!HAL_GPIO_ReadPin(HARD_BSPD_GPIO_Port, HARD_BSPD_Pin)) {
//	  report_fault(HARD_BSPD);
//	}

	if (mc_fault) {
	  report_fault(MC_FAULT);
	}

//  	if (!init_fault_cleared && mc_fault) {
//  		can_clear_MC_fault(&hcan1);
//  	  if (mc_fault_clear_success) {
//  		 init_fault_cleared = 1;
//  	  }
//  	}

	// send driver inputs to raspi on change OR if configured to constantly send
	if (ALWAYS_SEND_DRIVER_INPUTS ||
		(torque_percentage != prev_torque_percentage || launch_control_param != prev_launch_control_param) ) {
		can_tx_buttons_and_knobs(&hcan1);
	}

	switch (state) {
		case LV_LOCK:
			run_calibration();

			if (!is_switch_on(HV_SWITCH) && !is_switch_on(DRIVE_SWITCH)) {
			  change_state(LV);
			}
			break;
		case LV:
			run_calibration();

			// check if APPS pedal was calibrated
			if(!sensors_calibrated()){
				report_fault(UNCALIBRATED);
				break;
			}

			if (is_switch_on(HV_SWITCH)) {
				add_apps_deadzone();
				precharge_tick_start = HAL_GetTick();
				change_state(PRECHARGING);
				break;
			}

			break;
		case PRECHARGING:
			// Driver turned off HV via button
			if (!is_switch_on(HV_SWITCH)) {
				change_state(LV);
				break;
			}

			if((HAL_GetTick() - precharge_tick_start) > PRECHARGE_TIMEOUT_MS){
				report_fault(PRECHARGE_TIMEOUT);
				break;
			}

		  // if main AIRs closed
			if ((shutdown_flags & 0b110) == 0b110) {
				// Finished charging to HV on time
				change_state(HV_ENABLED);
				break;
			}

			break;
		case HV_LOCK:
			if (!is_switch_on(HV_SWITCH)) {
				// Driver flipped off HV switch
				change_state(LV);
				break;
			}

			if(!is_switch_on(DRIVE_SWITCH)){
				// wait until drive is low to switch into true HV
				change_state(HV_ENABLED);
			}
			break;
		case HV_ENABLED:
			// driver turned off HV
			if (!is_switch_on(HV_SWITCH)) {// || capacitor_volt < PRECHARGE_THRESHOLD) { // don't really need volt check by rules
				change_state(LV);
				break;
			}

			// driver attempt to go to drive mode
			if (is_switch_on(DRIVE_SWITCH)) {
				if (brake_mashed()) {
					change_state(DRIVE);
				}
				else {
					// driver didn't press brake
					report_fault(BRAKE_NOT_PRESSED);
				}
			}

			break;
		case DRIVE:
			// driver turned off drive
			if (!is_switch_on(DRIVE_SWITCH)) {
				change_state(HV_ENABLED);
				break;
			}

			if (!is_switch_on(HV_SWITCH)) {// || capacitor_volt < PRECHARGE_THRESHOLD) { // don't really need volt check by rules
				// driver turned off HV
				change_state(LV);
				break;
			}

			if (is_brake_implausible()) {
				report_fault(BRAKE_IMPLAUSIBLE);
			}

			break;
		case FAULT:
			switch (error) {
				case BRAKE_NOT_PRESSED:
					if (!is_switch_on(HV_SWITCH)){
						change_state(LV);
						break;
					}

					if (!is_switch_on(DRIVE_SWITCH)) {
						// reset drive switch and try again
						change_state(HV_ENABLED);
					}
					break;
				case SENSOR_DISCREPANCY:
					// stop power to motors if discrepancy persists for >100ms
					// see rule T.4.2.5 in FSAE 2022 rulebook
					if (!is_switch_on(DRIVE_SWITCH)) {
						discrepancy_timer_ms = 0;
						change_state(HV_ENABLED);
					}

					if (!is_switch_on(HV_SWITCH))
						change_state(LV);

					break;
				case BRAKE_IMPLAUSIBLE:
					if (!is_switch_on(HV_SWITCH)){
						change_state(LV);
						break;
					}

					if (!is_switch_on(DRIVE_SWITCH)){
						change_state(HV_ENABLED);
						break;
					}

					if (!is_brake_implausible()){
						change_state(DRIVE);
						break;
					}

					break;
				case SHUTDOWN_CIRCUIT_OPEN:
					if (shutdown_closed()) {
						change_state(LV); // change to startup so we don't instantly request precharge
					}
					break;
				case HARD_BSPD:
				  //should not be recoverable, but let hardware decide this
				  /*if (!HAL_GPIO_ReadPin(HARD_BSPD_GPIO_Port, HARD_BSPD_Pin)) {
					  change_state(LV);
				  }*/
					break;

				case UNCALIBRATED:
					run_calibration();

					// check if APPS pedal was calibrated
					if(sensors_calibrated()){
						change_state(LV); // change to startup so we don't instantly request precharge
						break;
					}
					break;
				case MC_FAULT:
					if (!mc_fault) {
						change_state(LV);
					}
					break;
				case PRECHARGE_TIMEOUT:
					// give time for fault message to show on display before going to LV
					if ((HAL_GetTick() - precharge_tick_start) > PRECHARGE_TIMEOUT_MS+1000) {
						change_state(LV);
					}
					break;
				default:
					break;
			}
		break;
	  }

	//HAL_GPIO_TogglePin(HEARTBEAT_GPIO_Port, HEARTBEAT_Pin);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc3.Init.Resolution = ADC_RESOLUTION_12B;
  hadc3.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc3.Init.ContinuousConvMode = DISABLE;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc3.Init.NbrOfConversion = 1;
  hadc3.Init.DMAContinuousRequests = DISABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 18;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_2TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_3TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */
	/*##-2- Configure the CAN Filter ###########################################*/
	CAN_FilterTypeDef canfilterconfig;

	canfilterconfig.FilterActivation = CAN_FILTER_ENABLE;
	canfilterconfig.FilterBank = 18;  // which filter bank to use from the assigned ones
	canfilterconfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	canfilterconfig.FilterIdHigh = 0x0;
	canfilterconfig.FilterIdLow = 0x0;
	canfilterconfig.FilterMaskIdHigh = 0x0;
	canfilterconfig.FilterMaskIdLow = 0x0000;
	canfilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;
	canfilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;
	canfilterconfig.SlaveStartFilterBank = 20;  // how many filters to assign to the CAN1 (master can)

	if (HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig) != HAL_OK)
	{
	  /* Filter configuration Error */
	  Error_Handler();
	}

	/*##-3- Start the CAN peripheral ###########################################*/
	if (HAL_CAN_Start(&hcan1) != HAL_OK)
	{
	  /* Start Error */
	  Error_Handler();
	}

	/*##-4- Activate CAN RX notification #######################################*/
	if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{
	  /* Notification Error */
	  Error_Handler();
	}

  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief CAN2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN2_Init(void)
{

  /* USER CODE BEGIN CAN2_Init 0 */

  /* USER CODE END CAN2_Init 0 */

  /* USER CODE BEGIN CAN2_Init 1 */

  /* USER CODE END CAN2_Init 1 */
  hcan2.Instance = CAN2;
  hcan2.Init.Prescaler = 18;
  hcan2.Init.Mode = CAN_MODE_NORMAL;
  hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan2.Init.TimeSeg1 = CAN_BS1_2TQ;
  hcan2.Init.TimeSeg2 = CAN_BS2_3TQ;
  hcan2.Init.TimeTriggeredMode = DISABLE;
  hcan2.Init.AutoBusOff = DISABLE;
  hcan2.Init.AutoWakeUp = DISABLE;
  hcan2.Init.AutoRetransmission = DISABLE;
  hcan2.Init.ReceiveFifoLocked = DISABLE;
  hcan2.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN2_Init 2 */

  if (HAL_CAN_Start(&hcan2) != HAL_OK) {
	  Error_Handler();
  }

  /* USER CODE END CAN2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(HEARTBEAT_GPIO_Port, HEARTBEAT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BUZZER_Pin */
  GPIO_InitStruct.Pin = BUZZER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUZZER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CURRENT_DIGITAL_Pin HARD_BSPD_Pin EXTRA_SENS2_Pin */
  GPIO_InitStruct.Pin = CURRENT_DIGITAL_Pin|HARD_BSPD_Pin|EXTRA_SENS2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : EXTRA_SENS1_Pin */
  GPIO_InitStruct.Pin = EXTRA_SENS1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(EXTRA_SENS1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : HEARTBEAT_Pin */
  GPIO_InitStruct.Pin = HEARTBEAT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(HEARTBEAT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BUTTON4_Pin BUTTON3_Pin BUTTON2_Pin BUTTON1_Pin
                           HV_REQUEST_Pin DRIVE_REQUEST_Pin */
  GPIO_InitStruct.Pin = BUTTON4_Pin|BUTTON3_Pin|BUTTON2_Pin|BUTTON1_Pin
                          |HV_REQUEST_Pin|DRIVE_REQUEST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
