/*
 * telem.h
 *
 *  Created on: Apr 26, 2024
 *      Author: nickr
 */

#ifndef INC_TELEM_H_
#define INC_TELEM_H_

#define PACKET_LENGTH 16

#define TELEM_DELAY 2

typedef struct __attribute__((__packed__)) Packet {
	uint8_t validation[2];
	uint16_t data_id;
	uint16_t data[4];
	uint32_t time;
} Packet;

extern volatile float current_slip_ratio;

// Function prototypes
void telem_send(void);

#endif /* INC_TELEM_H_ */
