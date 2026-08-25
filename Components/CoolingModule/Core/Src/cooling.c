#include "cooling.h"
#include "adc.h"
#include "pwm.h"
#include "can_manager.h"
#include "main.h"

#define VOLTAGE_DIVIDER_RATIO (12.0 / (12.0 + 6.04))
#define PSI_PER_KPA 0.145038
#define HI8(x) ((x>>8)&0xFF)
#define LO8(x) (x&0xFF);

#define NUM_SAMPLES_IN_AVERAGE 5

#define PUMP_THRESH 400
#define FAN_THRESH_1 400
#define FAN_THRESH_2 500
#define FAN_THRESH_3 600
#define HYSTERESIS 30

// HANDLE TYPE DEFS from main
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim1;

extern CAN_DATA_t can_data;

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

extern uint16_t ADC_RES_BUFFER[4]; // buffer for ADC DMA

// PRIVATE GLOBALS

PWM_Output_t pwm_fan;
PWM_Output_t pwm_pump;
PWM_Output_t pwm_extra;

uint8_t num_samples;

// average ADC readings
uint16_t adc_temp0 = 0;
uint16_t adc_temp1 = 0;
uint16_t adc_temp2 = 0;
uint16_t adc_temp3 = 0;


// PRIVATE FUNCTION PROTOTYPES
//uint16_t get_pres(uint16_t adc_val);
int16_t get_temp(uint16_t adc_val);
int16_t get_air_temp(uint16_t adc_val);
void set_fan_speed(uint8_t speed);
void set_pump_speed(uint8_t speed);
void update_pwm(int16_t inlet_temp);

void Cooling_Init(){
	PWM_Init(&pwm_fan, &htim1, TIM_CHANNEL_1);
	PWM_Init(&pwm_pump, &htim1, TIM_CHANNEL_2);
	PWM_Init(&pwm_extra, &htim1, TIM_CHANNEL_3);

	adc_temp0 = 0;
	adc_temp1 = 0;
	adc_temp2 = 0;
	adc_temp3 = 0;

	set_pump_speed(255);
	set_fan_speed(128);
}

void Cooling_Update()
{
	update_pwm(can_data.mc_temp_max);
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
	adc_temp0 = ADC_RES_BUFFER[0];
	adc_temp1 = ADC_RES_BUFFER[1];
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	adc_temp2 = ADC_RES_BUFFER[2];
	adc_temp3 = ADC_RES_BUFFER[3];

	CLEAR_BIT(hadc->Instance->SR, ADC_SR_EOC);
}

void CAN_Send_Temp_ADC(CAN_HandleTypeDef *hcan) {
	uint8_t tx_data[8];

	tx_data[0] = HI8(adc_temp0);
	tx_data[1] = LO8(adc_temp0);
	tx_data[2] = HI8(adc_temp1);
	tx_data[3] = LO8(adc_temp1);
	tx_data[4] = HI8(adc_temp2);
	tx_data[5] = LO8(adc_temp2);
	tx_data[6] = HI8(adc_temp3);
	tx_data[7] = LO8(adc_temp3);

	CAN_Send(hcan, COOLING_LOOP_TEMPS, tx_data, 8);
}

void update_pwm(int16_t inlet_temp)
{
	// allow cooling override: use hardcoded values for pump and fan speed
	if (can_data.PWM_requested) {
		set_pump_speed(128);
		set_fan_speed(128);
		return;
	}

	// threshold variables to add hysteresis
	static int fan_t1 = FAN_THRESH_1 + HYSTERESIS;
	static int fan_t2 = FAN_THRESH_2 + HYSTERESIS;
	static int fan_t3 = FAN_THRESH_3 + HYSTERESIS;
	static int pump_t = PUMP_THRESH + HYSTERESIS;

	//TODO: update these values to consider ambient air temp, vehicle speed, etc?
	if(can_data.inverter_enable || (can_data.mc_temp_max > pump_t) || (can_data.motor_temp > pump_t)){
		set_pump_speed(255);
		set_fan_speed(128);
		pump_t = PUMP_THRESH;
	} else {
		set_pump_speed(0);
		set_fan_speed(0);
		pump_t = PUMP_THRESH + HYSTERESIS;
	}

	// TODO: uncomment after thermistor calibration
	if(inlet_temp > fan_t3){
		set_fan_speed(255);
		fan_t1 = FAN_THRESH_1;
		fan_t2 = FAN_THRESH_2;
		fan_t3 = FAN_THRESH_3;
	} else if(inlet_temp > fan_t2){
		set_fan_speed(180);
		fan_t1 = FAN_THRESH_1;
		fan_t2 = FAN_THRESH_2;
		fan_t3 = FAN_THRESH_3 + HYSTERESIS;
	} else if(inlet_temp > fan_t1){
		fan_t1 = FAN_THRESH_1;
		fan_t2 = FAN_THRESH_2 + HYSTERESIS;
		fan_t3 = FAN_THRESH_3 + HYSTERESIS;
		set_fan_speed(150);
	} else {
		fan_t1 = FAN_THRESH_1 + HYSTERESIS;
		fan_t2 = FAN_THRESH_2 + HYSTERESIS;
		fan_t3 = FAN_THRESH_3 + HYSTERESIS;
	}
}

int16_t get_temp(uint16_t adc_val)
{
	float volts = adc_val * 3.3 / 4095;
	float temp_c = (-19.2 * volts * volts) + (33.3 * volts) + 79.7;
	return (int16_t)(temp_c * 10);
}


void set_pump_speed(uint8_t speed)
{
	PWM_SetDutyCycle(&pwm_pump, speed);
}

void set_fan_speed(uint8_t speed)
{
	PWM_SetDutyCycle(&pwm_fan, speed);
}

/* Code for continuous ADC scan
// ISR called when ADC finishes half of the conversions and DMA has written them to ADC_RES_BUFFER
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
	if (num_samples == 0) { // first sample of this average
		adc_temp0_average = ADC_RES_BUFFER[0];
		adc_temp1_average = ADC_RES_BUFFER[1];
	} else if (num_samples < NUM_SAMPLES_IN_AVERAGE) {
		// calculate running average
		// NewAverage = OldAverage * (n-1) / n + NewValue / n, where n is num elements AFTER new element included
		adc_temp0_average = adc_temp0_average * (num_samples-1) / num_samples + ADC_RES_BUFFER[0] / num_samples;
		adc_temp1_average = adc_temp1_average * (num_samples-1) / num_samples + ADC_RES_BUFFER[1] / num_samples;
	}
}

// ISR called when ADC finishes all conversions and DMA has written to ADC_RES_BUFFER
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	static uint8_t tx_data[8];
	if (num_samples == 0) { // first sample of this average
		num_samples++;

		adc_temp2_average = ADC_RES_BUFFER[2];
		adc_temp3_average = ADC_RES_BUFFER[3];
	} else if (num_samples < NUM_SAMPLES_IN_AVERAGE) {
		// calculate running average
		// NewAverage = OldAverage * (n-1) / n + NewValue / n, where n is num elements AFTER new element included
		num_samples++;

		adc_temp2_average = adc_temp2_average * (num_samples-1) / num_samples + ADC_RES_BUFFER[2] / num_samples;
		adc_temp3_average = adc_temp3_average * (num_samples-1) / num_samples + ADC_RES_BUFFER[3] / num_samples;
	} else {

		if (can_loop_counter > CAN_LOOP_DELAY) {
			can_loop_counter = 0;
			// send over can
			int16_t temp0 = adc_temp0_average;
			int16_t temp1 = adc_temp1_average;
			int16_t temp2 = adc_temp2_average;
			int16_t temp3 = adc_temp3_average;

			tx_data[0] = HI8(temp0);
			tx_data[1] = LO8(temp0);
			tx_data[2] = HI8(temp1);
			tx_data[3] = LO8(temp1);
			tx_data[4] = HI8(temp2);
			tx_data[5] = LO8(temp2);
			tx_data[6] = HI8(temp3);
			tx_data[7] = LO8(temp3);
			CAN_Send(&hcan1, COOLING_LOOP_TEMPS, tx_data, 8); // TODO: change to hcan2 after testing that DAQ works
		} else {
			can_loop_counter++;
		}

		// reset averages and num_samples
		adc_temp0_average = 0;
		adc_temp1_average = 0;
		adc_temp2_average = 0;
		adc_temp3_average = 0;
		num_samples = 0;
	}
}
 */
