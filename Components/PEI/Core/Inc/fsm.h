/*
 * fsm.h
 *
 *  Created on: Sep 13, 2024
 *      Author: Abhineet
 */

#ifndef INC_FSM_H_
#define INC_FSM_H_

#include <stdint.h>

typedef enum {
    PEI_LV,
    PEI_PRECHARGE,
    PEI_HV,
    PEI_FAULT
} PEI_STATE_t;

typedef enum {
    LV,
    PRECHARGING,
    HV_ENABLED,
    DRIVE,
	CHARGING,
    VCU_ERROR = 0x80
} VCU_STATE_t;

typedef enum {
    VSM_START,
    PRECHARGE_INIT,
    PRECHARGE_ACTIVE,
    PRECHARGE_COMPLETE,
    VSM_WAIT,
    VSM_READY,
    MOTOR_RUNNING,
    BLINK_FAULT_CODE,
    SHUTDOWN_IN_PROCESS = 14,
    RECYCLE_POWER = 15
} MC_VSM_STATE_t;

typedef enum {
    DISCHARGE_DISABLED,
    DISCHARGE_ENABLED,
    SPEED_CHECK,
    DISCHARGE_ACTIVE,
    DISCHARGE_COMPLETE
} MC_DISCHARGE_STATE_t;

#define NORMAL 0x00
#define CHARGER_TIMEOUT 0x01
#define CHARGER_FAULT 0x02
#define VCU_TIMEOUT 0x04
#define VCU_FAULT 0x08
#define MC_TIMEOUT 0x10
#define MC_FAULT 0x20
#define SHUTDOWN 0x40
#define MC_DISCHARGING 0x80

uint8_t hv_request();
uint8_t hv_allowed();
uint8_t precharge_ready();
uint8_t precharge_complete();

void update_status();

#endif /* INC_FSM_H_ */
