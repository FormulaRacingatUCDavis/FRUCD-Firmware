/*
 * battery_gui.c
 *
 *  Created on: Dec 6, 2024
 *      Author: Abhineet
 */


#include "battery_gui.h"
#include "cell_interface.h"
#include "charger.h"
#include "utils.h"

static const uint8_t TIMEOUT = 100;

static const uint8_t ESCAPE_CHAR = 0xAA;
static const uint8_t PACK_FRAME_START = 0xBB;
static const uint8_t FRAME_END = 0x0A;

extern volatile BAT_PACK_t bat_pack;

extern const Charge_Profile_t ESDC;
extern const Charge_Profile_t COMP;

extern Charge_Profile_t selected_profile;

static void send_byte(UART_HandleTypeDef* const huart, uint8_t byte) {
	HAL_UART_Transmit(huart, &byte, 1, TIMEOUT);
}

// Sends extra escape byte if byte is an escape byte
static void send_byte_with_escape(UART_HandleTypeDef* const huart, uint8_t byte) {
	if (byte == ESCAPE_CHAR) send_byte(huart, ESCAPE_CHAR);
	send_byte(huart, byte);
}

void uart_send_GUI_Data(UART_HandleTypeDef* const huart) {
	uint8_t i = 0;
	uint8_t j = 0;

	for(i = 0; i < N_OF_SUBPACK; i++){

		send_byte(huart, ESCAPE_CHAR);
		send_byte(huart, i);

		for(j = 0; j < CELLS_PER_SUBPACK; j++){
			uint16_t v = (uint16_t)bat_pack.subpacks[i].cells[j].voltage_raw;
			send_byte_with_escape(huart, HI8(v));
			send_byte_with_escape(huart, LO8(v));
		}
		for(j = 0; j < CELL_TEMPS_PER_SUBPACK; j++){
			uint8_t t = (uint8_t)bat_pack.subpacks[i].cell_temps[j].temp_c;
			send_byte_with_escape(huart, t);
		}

		send_byte(huart, ESCAPE_CHAR);
		send_byte(huart, FRAME_END);
	}

	send_byte(huart, ESCAPE_CHAR);
	send_byte(huart, PACK_FRAME_START);

	uint16_t v = (uint16_t)bat_pack.pack_voltage_raw;
	send_byte_with_escape(huart, HI8(v));
	send_byte_with_escape(huart, LO8(v));

	v = (uint16_t)bat_pack.LO_voltage_raw;
	send_byte_with_escape(huart, HI8(v));
	send_byte_with_escape(huart, LO8(v));

	v = (uint16_t)bat_pack.HI_voltage_raw;
	send_byte_with_escape(huart, HI8(v));
	send_byte_with_escape(huart, LO8(v));

	uint8_t t = (uint8_t)bat_pack.HI_temp_c;
	send_byte_with_escape(huart, t);

	t = (uint8_t)bat_pack.LO_temp_c;
	send_byte_with_escape(huart, t);

	t = (uint8_t)bat_pack.AVG_temp_c;
	send_byte_with_escape(huart, t);

	send_byte_with_escape(huart, bat_pack.SOC_percent);

	send_byte_with_escape(huart, bat_pack.status);

	send_byte(huart, ESCAPE_CHAR);
	send_byte(huart, FRAME_END);
}

// Returns if a charge profile choice was received from the GUI
uint8_t uart_receive_charge_profile(UART_HandleTypeDef* const huart) {
	static uint8_t rx_buff[1];

	if (HAL_UART_Receive(huart, rx_buff, 1, 100) == HAL_OK) {
		if (rx_buff[0] == 1) selected_profile = COMP;
		else selected_profile = ESDC;

		return 1;
	}

	return 0;
}
