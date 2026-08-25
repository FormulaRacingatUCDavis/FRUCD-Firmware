/*
 * config.h
 *
 *  Created on: Apr 12, 2024
 *      Author: leoja
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

// assuming PSC = 7199, one tick is 0.1 ms
// 	e.g. 1000 timer ticks * 0.1ms = 100ms
// 	so we would send CAN at a max of 10 Hz
#define CAN_DELAY_TICKS 1000


#endif /* INC_CONFIG_H_ */
