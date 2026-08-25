#include "serial_print.h"
#include <string.h>
#include <stdio.h>
#include "stm32f7xx_hal.h"
#include "serial.h"
#include "can_manager.h"

extern UART_HandleTypeDef huart3;

void print(const char *str){
    static uint8_t first_run = 1;
    static Serial_t serial;

    if (first_run) {
        Serial_Init(&serial, &huart3, 0, 0);
        first_run = 0;
    }

    Serial_SendBytes(&serial, str, strlen(str), 20);
    //HAL_UART_Transmit(&huart3, str, strlen(str), 10);
}

void print_motor_data(){
    char buf[128];

    print("MOTOR DATA:\n");

    snprintf(buf, sizeof(buf),
             "Temp: %u\n"
             "Right Wheel: %u\n"
             "Left Wheel: %u\n"
             "Wheel Update?: %u\n",
             motor_temp,
             rear_right_wheel_speed,
             rear_left_wheel_speed,
             wheel_updated);

    print(buf);
}

void print_cooling_data(){
    char buf[128];

    print("COOLING DATA:\n");

    snprintf(buf, sizeof(buf),
             "Inlet Pres: %d\n"
             "Outlet Pres: %d\n"
             "Inlet Temp: %d\n"
             "Outlet Temp: %d\n",
             inlet_pres,
             outlet_pres,
             inlet_temp,
             outlet_temp);

    print(buf);
}

void print_pei_flags(){
	char buf[128];

	snprintf(buf, sizeof(buf),
	         "IMD OK: %u\n"
			 "BMS_OK: %u\n"
			 "Shutdown: %u\n"
			 "AIR NEG: %u\n"
			 "AIR POS: %u\n"
			 "Precharge: %u\n",
	         (shutdown_flags >> 5) & 1,
			 (shutdown_flags >> 4) & 1,
			 (shutdown_flags >> 3) & 1,
			 (shutdown_flags >> 2) & 1,
			 (shutdown_flags >> 1) & 1,
			 shutdown_flags & 1);

	print(buf);
}

void print_bms_data(void){
    char buf[128];

    print("BMS DATA:\n");

    snprintf(buf, sizeof(buf),
             "Pack Temp: %u\n"
             "Pack Voltage: %u\n"
             "Charge State: %u\n"
             "BMS Status: %u\n",
             PACK_TEMP,
             pack_voltage,
             soc,
             bms_status);

    print(buf);
}

void print_mc_data(void){
    char buf[128];

    print("MC DATA:\n");

    snprintf(buf, sizeof(buf),
             "Lockout: %u\n"
             "Enabled: %u\n"
             "Fault: %u\n"
             "Fault Clear Success: %u\n"
             "MC Temp: %d\n"
             "Cap Volt (x10): %d\n"
             "GLV Volt: %d\n"
             "Max Power: %u\n",
             mc_lockout,
             mc_enabled,
             mc_fault,
             mc_fault_clear_success,
             mc_temp,
             capacitor_volt_x10,
             glv_v,
             max_power);

    print(buf);
}
