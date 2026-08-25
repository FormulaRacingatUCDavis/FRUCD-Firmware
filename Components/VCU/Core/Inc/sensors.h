/*
 * sensors.h
 *
 *  Created on: Feb 20, 2024
 *      Author: cogus
 */

#ifndef SRC_SENSORS_H_
#define SRC_SENSORS_H_

#ifndef _XTAL_FREQ
#define _XTAL_FREQ  8000000UL
#endif

#ifndef FCY
#define FCY 8000000UL
#endif

#include <stdint.h>
#include <stdbool.h>

#include "stm32f7xx_hal.h"

#include "config.h"
#include "fsm.h"

// There is some noise when reading from the brake pedal
// So give some room for error when driver presses on brake
// UNUSED FOR NOW?
#define BRAKE_ERROR_TOLERANCE 50

typedef struct{
    uint16_t raw;
    uint16_t min;
    uint16_t max;
    uint16_t range;
    uint16_t percent;
} CALIBRATED_SENSOR_t;


extern CALIBRATED_SENSOR_t throttle1;
extern CALIBRATED_SENSOR_t throttle2;
extern CALIBRATED_SENSOR_t brake1;
extern CALIBRATED_SENSOR_t brake2;
extern CALIBRATED_SENSOR_t brake;
extern volatile uint32_t torque_percentage;
extern volatile uint32_t launch_control_param;
extern volatile uint32_t regen_braking_param;

extern volatile uint32_t lv_battery_measure_adc;

typedef enum {
	APPS1,
	APPS2,
	BSE1,
	BSE2,
	KNOB1,
	KNOB2,
	LV_BAT_MEASURE
} ADC_CHAN;


/************ Timer ************/
extern unsigned int discrepancy_timer_ms;

//function prototypes
uint32_t get_adc_conversion(ADC_HandleTypeDef *hadc, ADC_CHAN channel);
void run_calibration();
void update_sensor_vals(ADC_HandleTypeDef *hadc1, ADC_HandleTypeDef *hadc3);

bool sensors_calibrated();
bool has_discrepancy();
bool is_brake_implausible();
bool braking();
bool brake_mashed();

void temp_attenuate();
int16_t requested_throttle();

float raw_to_mvolts(uint16_t adc_raw);
float mvolts_to_amps(float mVolts, float mVolt_ref);

int16_t clamp(int16_t in, int16_t min, int16_t max);
void update_percent(CALIBRATED_SENSOR_t* sensor);
void update_brake();
void update_minmax(CALIBRATED_SENSOR_t* sensor);
void add_apps_deadzone();
void add_deadzone(CALIBRATED_SENSOR_t* sensor, uint16_t deadzone_percentage);
void init_sensors();



#endif /* SRC_SENSORS_H_ */
