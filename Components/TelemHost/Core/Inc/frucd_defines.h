/*
 * frucd_defines.h
 *
 *  Created on: Jan 30, 2024
 *      Author: leoja
 */

#ifndef INC_FRUCD_DEFINES_H_
#define INC_FRUCD_DEFINES_H_

//typedef enum{
//    LV,
//    PRECHARGING,
//    HV_ENABLED,
//    DRIVE,
//    FAULT = 0x80,
//} vcu_state;
//
//typedef enum {
//    NONE,
//    DRIVE_REQUEST_FROM_LV,
//    CONSERVATIVE_TIMER_MAXED,
//    BRAKE_NOT_PRESSED,
//    HV_DISABLED_WHILE_DRIVING,
//    SENSOR_DISCREPANCY,
//    BRAKE_IMPLAUSIBLE,
//    SHUTDOWN_CIRCUIT_OPEN,
//    UNCALIBRATED,
//    HARD_BSPD,
//    MC_FAULT
//} vcu_fault;

typedef enum{
//    NO_ERROR = 0x0000,
//    CHARGEMODE = 0x0001,
//    PACK_TEMP_OVER = 0x0002,
//    FUSE_BLOWN = 0x0004,
//    PACK_TEMP_UNDER = 0x0008,
//    LOW_SOC = 0x0010,
//    CRITICAL_SOC = 0x0020,
//    IMBALANCE = 0x0040,
//    COM_FAILURE = 0x0080,
//    NEG_CONT_CLOSED = 0x0100,
//    POS_CONT_CLOSED = 0x0200,
//    ISO_FAULT = 0x0400,
//    SPI_FAULT = 0x0400,
//    CELL_VOLT_OVER = 0x0800,
//    CELL_VOLT_UNDER = 0x1000,
//    CHARGE_HAULT = 0x2000,
//    FULL = 0x4000,
//    PRECHARGE_CLOSED = 0x8000


	// BMS Status Macros
	NO_ERROR = 0x00,
	CHARGEMODE = 0x01,
	PACK_TEMP_OVER = 0x02,
	PACK_TEMP_UNDER = 0x04,
	CELL_VOLT_OVER = 0x08,
	CELL_VOLT_UNDER = 0x10,
	OPEN_WIRE = 0x20,
	MISMATCH = 0x40,
	SPI_FAULT = 0x80
} BMS_STATUS;

#endif /* INC_FRUCD_DEFINES_H_ */
