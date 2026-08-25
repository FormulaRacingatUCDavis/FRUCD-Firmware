/*
 * can_manager.h
 *
 *  Created on: Apr 18, 2024
 *      Author: leoja
 */

#ifndef INC_CAN_MANAGER_H_
#define INC_CAN_MANAGER_H_

#include "stm32f4xx_hal.h"

typedef enum {
    // PCAN
    //BSPD_FLAGS = 0x0c1,
    VEHICLE_STATE = 0x766,
    TORQUE_REQUEST = 0x0C0,
	CLEAR_FAULT = 0x0C1,
    BMS_STATUS_MSG = 0x380,
    PEI_CURRENT_SHUTDOWN = 0x387,
    MC_VOLTAGE_INFO = 0x0A7,
    MC_INTERNAL_STATES = 0xAA,
    MC_FAULT_CODES = 0xAB,
	MC_TEMPS_1 = 0xA0,
	MC_TEMPS_2 = 0xA1,
	MC_TEMPS_3 = 0xA2,
	COOLING_LOOP_TEMPS = 0x400,
    WHEEL_SPEED_REAR = 0x401,
	COOLING_LOOP_PRESSURES = 0x402,
	STRAIN_GAUGES_REAR = 0x403,
	PUMP_PWM = 0x505,
} CAN_ID;

typedef enum {
    LV,
    PRECHARGING,
    HV_ENABLED,
    DRIVE,
	CHARGING,
    VCU_STATE_FAULT = 0x80
} VCU_STATE_t;

typedef struct {
	VCU_STATE_t vcu_state;
	uint8_t bms_temp;
	int16_t mc_temp_module_a;
	int16_t mc_temp_module_b;
	int16_t mc_temp_module_c;
	int16_t mc_temp_max;
	int16_t motor_temp;
	uint8_t inverter_enable;
	uint8_t PWM_requested;
	uint8_t pumpPWM;
	uint8_t fanPWM;
} CAN_DATA_t;

void CAN_Filter_Init();
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan_ptr);
HAL_StatusTypeDef CAN_Send(CAN_HandleTypeDef *hcan_ptr, uint32_t id, uint8_t* data, uint8_t len);

#endif /* INC_CAN_MANAGER_H_ */
