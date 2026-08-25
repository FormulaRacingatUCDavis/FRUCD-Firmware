/*
 * utils.c
 *
 *  Created on: Aug 23, 2024
 *      Author: Abhineet
 */


#include "utils.h"

uint8_t HI8(uint16_t word) {
	return (uint8_t)(word >> 8); // return upper 8 bits of 16-bit data
}

uint8_t LO8(uint16_t word) {
	return (uint8_t)(word & 0x00FF); // return lower 8 bits of 16-bit data
}

uint8_t HI4(uint8_t byte) {
	return byte >> 4; // return upper 4 bits of a byte
}

uint8_t LO4(uint8_t byte) {
	return byte & 0x0F; // return lower 4 bits of a byte
}

// Copy an array into another array
void copy_array_into(uint8_t buf[], uint8_t data[], uint8_t len) {
	for (uint8_t i = 0; i < len; i++) {
		buf[i] = data[i];
	}
}
