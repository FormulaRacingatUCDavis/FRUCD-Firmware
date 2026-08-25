#include "traction_control.h"
#include "can_manager.h"
#include "stdio.h"
#include "config.h"

#define MIN_FRONT_SPEED 50  // slip ratio calculation doesn't work for zero
#define MAX_TORQUE_REQUEST (MAX_TORQUE_NM * 10)  // torque requests are in Nm * 10

volatile float TC_control_var = 0;
volatile uint16_t TC_torque_req = 0; // torque request in Nm for best consistency

volatile float pid_error = 0;
volatile float prev_pid_error = 0;

volatile float integral = 0;
const float integral_cap = 10; // PLACEHOLDER
volatile float derivative = 0;

const float pi = 3.14;
const float wheel_radius = 0.5; // PLACEHOLDER VALUE
const uint16_t pulses_per_rev = 60; // from wheel speed sensor

float target_slip_ratio = 0.17;
volatile float current_slip_ratio = 0;

const float kP = 1500;
const float kI = 400;
const float kD = 0; // probably don't need this term

void traction_control_PID(uint32_t fr_wheel_speed, uint32_t fl_wheel_speed) {
    //if (state != DRIVE) return;

    // units are in RPM/CPS
    float avg_front_wheel_speed = ((float)(fr_wheel_speed));// + fl_wheel_speed))/2.0;
    float avg_rear_wheel_speed = ((float)rear_right_wheel_speed)*12.0/33.0; // (back_right_wheel_speed + back_left_wheel_speed)/2.0;

    if(avg_front_wheel_speed < MIN_FRONT_SPEED){
    	avg_front_wheel_speed = MIN_FRONT_SPEED;
    }

    current_slip_ratio = avg_rear_wheel_speed/avg_front_wheel_speed - 1;

//    // calculate dynamic slip ratio
//    target_slip_ratio = 0.1 - 0.01*(avg_back_wheel_speed/max_wheel_speed);

    // if target slip ratio has been achieved
    //if (current_slip_ratio < target_slip_ratio + 0.001 && current_slip_ratio > target_slip_ratio - 0.001) {
    	//integral = 0; // reset integral
    	//return;
    //}

    // anti-windup
    if (integral > integral_cap) integral = integral_cap;
    if(integral < 0) integral = 0;

    pid_error = current_slip_ratio - target_slip_ratio;
    integral = integral + pid_error;
    derivative = pid_error - prev_pid_error;

    TC_control_var = (kP * pid_error) + (kI * integral) + (kD * derivative);

    // limit PID torque request
    if (TC_control_var > MAX_TORQUE_REQUEST) TC_control_var = MAX_TORQUE_REQUEST;
    if (TC_control_var < 0) TC_control_var = 0;

    TC_torque_req = MAX_TORQUE_REQUEST - (uint16_t)TC_control_var;

    prev_pid_error = pid_error;
}
