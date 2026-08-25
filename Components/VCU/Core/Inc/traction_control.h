/*
 * traction_control.h
 *
 *  Created on: Feb 20, 2024
 *      Author: cogus
 */

#include <stdint.h>
#include "fsm.h"

#ifndef SRC_TRACTION_CONTROL_H_
#define SRC_TRACTION_CONTROL_H_

extern volatile float TC_control_var;
extern volatile uint16_t TC_torque_req;
extern volatile float current_slip_ratio;

extern volatile float pid_error;
extern volatile float prev_pid_error;

extern volatile float integral;
extern volatile float derivative;

void traction_control_PID(uint32_t fr_wheel_speed, uint32_t fl_wheel_speed);



#endif /* SRC_TRACTION_CONTROL_H_ */
