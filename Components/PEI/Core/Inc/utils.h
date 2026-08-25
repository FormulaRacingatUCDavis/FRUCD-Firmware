/*
 * utils.h
 *
 *  Created on: Aug 23, 2024
 *      Author: Abhineet
 */

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include <stdint.h>

uint8_t HI8(uint16_t word);
uint8_t LO8(uint16_t word);

uint8_t HI4(uint8_t byte);
uint8_t LO4(uint8_t byte);

void copy_array_into(uint8_t* buf, uint8_t* data, uint8_t len);

#endif /* INC_UTILS_H_ */
