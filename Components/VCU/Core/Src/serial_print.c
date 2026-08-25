#include "serial_print.h"
#include <stdio.h>
#include <string.h>
#include "stm32f7xx_hal.h"
#include "serial.h"
#include "driver_input.h"
#include "can_manager.h"
#include "sensors.h"

extern UART_HandleTypeDef huart3;

void print(const char *str) {
	static uint8_t first_run = 1;
	static Serial_t serial;

	if (first_run) {
		Serial_Init(&serial, &huart3, 0, 0);
		first_run = 0;
	}

	Serial_SendBytes(&serial, (const uint8_t*)str, strlen(str), 20);

//	HAL_UART_Transmit(&huart3, str, strlen(str), 10);
}


void serial_print_vehicle_state(void) {
	char buf[128];
	print("==VEHICLE STATE DATA==\n");
	snprintf(buf, sizeof(buf),
			"MC_LOCKOUT: %s\n"
			"MC_ENABLED: %s\n",
			mc_lockout ? "True" : "False",
			mc_enabled ? "True" : "False") ;
	print(buf);

	print("==SHUTDOWN FLAGS==\n");
	snprintf(buf, sizeof(buf),
			"IMD: %s\n"
			"BMS: %s\n"
			"SHUTDOWN_FINAL: %s\n"
			"AIR_NEG: %s\n"
			"AIR_POS: %s\n"
			"PRECHARGE: %s\n",
			((shutdown_flags >> 5) & 1) ? "True" : "False",
			((shutdown_flags >> 4) & 1) ? "True" : "False",
			((shutdown_flags >> 3)& 1) ? "True" : "False",
			((shutdown_flags >> 2) & 1) ? "True" : "False",
		    ((shutdown_flags >> 1) & 1) ? "True" : "False",
		    (shutdown_flags & 1) ? "True" : "False");
	print(buf);

	print("DASHBOARD DISPLAY MODE:");

	switch(dashboard_display_mode){
	case DISPLAY_DRIVE:
		snprintf(buf, sizeof(buf), "DRIVE\n");
		break;

	case DISPLAY_DEBUG:
		snprintf(buf, sizeof(buf), "DEBUG\n");
		break;

	case DISPLAY_PRACTICE:
		snprintf(buf, sizeof(buf), "PRACTICE\n");
		break;

	print(buf);
	}
}

void serial_print_cooling(void) {
	char buf[128];

	print("==COOLING==");
	snprintf(buf, sizeof(buf),
			"INLET_TEMP: %u C\n"
			"OUTLET_TEMP: %u C\n"
			"INLET_PRES: %u PSI\n"
			"OUTLET_PRES: %u PSI\n",
			inlet_temp, outlet_temp, inlet_pres, outlet_pres);
	print(buf);
}

void serial_print_driver_input(void) {
	char buf[128];
	print("==DRIVER INPUT==");
	snprintf(buf,sizeof(buf),
			"ACC_CURRENT_ADC : %lu A\n"
			"ACC_CURRENT_REF_ADC : %u A\n"
			"SG_REAR : %u A\n"
			"LAUNCH_CTRL_PARAM : %lu A\n"
			"TORQUE_PERCENTAGE : %lu A\n",
			(unsigned long)acc_current_adc,
			acc_current_ref_adc,
			sg_rear,
			(unsigned long)launch_control_param,
			(unsigned long)torque_percentage);
	print(buf);
}
//launch control param is undeclared in the header file???

void dump_can_data_battery() {
	// soc, bms_status, pack_temp, acc_current_adc, acc_current_ref_adc, pack_voltage, glv_v
	// prints all the battery related info

	char buffer[100];

	print("BATTERY READINGS\n");

	sprintf(buffer, "SOC: %u%%\n", soc);
	print(buffer);

	switch (bms_status) {
		case 0:
			sprintf(buffer, "BMS_STATUS: NO ERROR\n");
			break;

		case 1:
			sprintf(buffer, "BMS_STATUS: CHARGE MODE\n");
			break;

		case 2:
			sprintf(buffer, "BMS_STATUS: BMS TEMP OVER\n");
			break;

		case 4:
			sprintf(buffer, "BMS_STATUS: BMS TEMP UNDER\n");
			break;

		case 8:
			sprintf(buffer, "BMS_STATUS: OVERVOLT\n");
			break;

		case 16:
			sprintf(buffer, "BMS_STATUS: UNDERVOLT\n");
			break;

		case 32:
			sprintf(buffer, "BMS_STATUS: OPEN WIRE\n");
			break;

		case 64:
			sprintf(buffer, "BMS_STATUS: MISMATCH\n");
			break;

		case 128:
			sprintf(buffer, "BMS_STATUS: SPI FAULT\n");
			break;
	}
	print(buffer);

	sprintf(buffer, "PACK_TEMP: %uC\n", PACK_TEMP);
	print(buffer);

	sprintf(buffer, "ACC_CURRENT_ADC: %u\n", acc_current_adc);
	print(buffer);

	sprintf(buffer, "ACC_CURRENT_REF_ADC %u\n", acc_current_ref_adc);
	print(buffer);

	snprintf(buffer, sizeof(buffer), "CURRENTS READING: %dA\n", (int)mvolts_to_amps(raw_to_mvolts(acc_current_adc), raw_to_mvolts(acc_current_ref_adc)));
	print(buffer);


	sprintf(buffer, "PACK VOLTAGE: %dV\n", pack_voltage);
	print(buffer);

	sprintf(buffer, "GLV_V: %d.%dV", glv_v / 100, glv_v % 100);
	print(buffer);

}

void dump_can_data_motor_controller() {
	//mcfault, mc temp, motor speed
	// prints all the motor controller related info
	char buffer[64];

	print("MOTOR CONTROLLER INFO\n");

	if (mc_fault) {
		sprintf(buffer, "MOTOR CONTROLLER FAULT\n");
		print(buffer);
	} else {
		sprintf(buffer, "NO MOTOR CONTROLLER FAULT\n");
		print(buffer);
	}

	sprintf(buffer, "MOTOR CONTROL TEMP: %u C\n", mc_temp);
	print(buffer);

	sprintf(buffer, "MOTOR SPEED: %d rpm\n", motor_speed);
	print(buffer);
}


