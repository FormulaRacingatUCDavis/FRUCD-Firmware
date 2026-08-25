/*
 * can_manager.h
 *
 *  Created on: Feb 20, 2024
 *      Author: cogus
 */


#ifndef SRC_CAN_MANAGER_H_
#define SRC_CAN_MANAGER_H_

#include <stdint.h>
#include "stm32f7xx_hal.h"
#include "fsm.h"
#include "sensors.h"


/********** ENUM OF CAN IDS **********/
typedef enum {
    VEHICLE_STATE = 0x766,
    TORQUE_REQUEST = 0x0C0,
//    BMS_STATUS_MSG = 0x380,
//    PEI_CURRENT_SHUTDOWN = 0x387,
	PEI_STATUS_MSG = 0x387,
	BMS_STATUS_MSG = 0x380,
	DIAGNOSTIC_BMS_DATA = 0x381,
    MC_VOLTAGE_INFO = 0x0A7,
    MC_INTERNAL_STATES = 0xAA,
    MC_FAULT_CODES = 0xAB,
	MC_PARAM_COMMAND = 0x0C1,
	MC_PARAM_RESPONSE = 0x0C2,
	MC_MOTOR_POSITION = 0x0A5,
	MC_TEMP_1 = 0xA0,
	MC_TEMP_3 = 0x0A2,
	MC_INTERNAL_VOLTS = 0x0A9,
	COOLING_LOOP = 0x400,
	WHEEL_SPEED_REAR = 0x401,
	COOLING_LOOP_PRESSURES = 0x402,
	STRAIN_GAUGE_REAR = 0x403,
	MC_INTERNAL_CURRENTS = 0x0A6,
	PEDALS_RAW = 0x502,
	MC_AC_POWER = 0x504
} CAN_ID;

extern volatile uint8_t mc_lockout;
extern volatile uint8_t mc_enabled;
extern volatile int16_t capacitor_volt_x10;
extern volatile uint8_t shutdown_flags;
extern volatile uint8_t estop_flags;
extern volatile uint8_t switches;
extern volatile uint8_t PACK_TEMP;
extern volatile uint8_t mc_fault;
extern volatile uint8_t soc;
extern volatile uint8_t bms_status;
extern volatile uint8_t mc_fault_clear_success;
extern volatile uint8_t dashboard_display_mode;
extern volatile int16_t pack_voltage;
extern volatile uint16_t motor_temp;
extern volatile uint16_t mc_temp;
extern volatile int16_t glv_v;

extern volatile int16_t motor_speed;
extern volatile uint16_t rear_right_wheel_speed;
extern volatile uint16_t rear_left_wheel_speed;
extern volatile uint16_t front_right_wheel_speed;
extern volatile uint16_t front_left_wheel_speed;
extern volatile uint8_t wheel_updated[2];
extern volatile int16_t inlet_temp;
extern volatile int16_t outlet_temp;
extern volatile int16_t inlet_pres;
extern volatile int16_t outlet_pres;
extern volatile uint16_t telem_id;
extern volatile uint16_t sg_rear;
extern volatile uint16_t max_power;
extern volatile uint16_t acc_current_adc;
extern volatile uint16_t acc_current_ref_adc;

// extern CAN_RxHeaderTypeDef RxHeader;
// extern uint8_t RxData[8];

// extern CAN_TxHeaderTypeDef   TxHeader;
// extern uint32_t              TxMailbox;

// void save_can_rx_data(CAN_RxHeaderTypeDef rxHeader, uint8_t rxData[]);
void can_tx_vcu_state(CAN_HandleTypeDef *hcan);
void can_tx_torque_request(CAN_HandleTypeDef *hcan);
void can_tx_disable_MC(CAN_HandleTypeDef *hcan);
void can_clear_MC_fault(CAN_HandleTypeDef *hcan);
void can_tx_sg(CAN_HandleTypeDef *hcan, uint16_t adc);
void can_tx_buttons_and_knobs(CAN_HandleTypeDef *hcan);
void can_tx_pedals_raw(CAN_HandleTypeDef *hcan);
void can_tx_power(CAN_HandleTypeDef *hcan);

// might want to have all the tx functions use this more generic function
HAL_StatusTypeDef CAN_Send(CAN_HandleTypeDef *hcan, uint32_t id, uint8_t* data, uint8_t len);

#endif /* SRC_CAN_MANAGER_H_ */

