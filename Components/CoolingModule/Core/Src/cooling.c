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


volatile ADC_Channel_t adc_channels[NUM_ADC_CHANNELS] = {
		{TEMP0_CHANNEL, 0, 0}, // TEMP0, 20 Hz
		{TEMP1_CHANNEL, 0, 0}, // TEMP1, 20 Hz
		{TEMP2_CHANNEL, 0, 0}, // TEMP2, 20 Hz
		{TEMP3_CHANNEL, 0, 0} // TEMP3, 20 Hz
};


// PRIVATE GLOBALS

PWM_Output_t pwm_fan;
PWM_Output_t pwm_pump;
PWM_Output_t pwm_extra;


// ADC readings
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

void Sensor_Update(CAN_HandleTypeDef *hcan) {
	// any of the temps
	if (adc_channels[0].flag == 1 || adc_channels[1].flag == 1
			|| adc_channels[2].flag == 1 || adc_channels[3].flag == 1) {
		adc_temp0 = adc_channels[0].adc_value;
		adc_temp1 = adc_channels[1].adc_value;
		adc_temp2 = adc_channels[2].adc_value;
		adc_temp3 = adc_channels[3].adc_value;

		CAN_Send_Temp_ADC(hcan);

		// reset flags
		adc_channels[0].flag = 0;
		adc_channels[1].flag = 0;
		adc_channels[2].flag = 0;
		adc_channels[3].flag = 0;
	}
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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2) // with PSC = 71 and ARR = 999, TIM2 runs at 1000 Hz
    {
    	static uint32_t ticks = 0; // 1000 Hz -> one tick is one ms

    	// 20 Hz -> Period of 50 ms
    	if (ticks % 50 == 0) {
    		adc_channels[0].flag = 1; // TEMP0
    		adc_channels[1].flag = 1; // TEMP1
    		adc_channels[2].flag = 1; // TEMP2
    		adc_channels[3].flag = 1; // TEMP3
    	}

		ticks++;
		// reset the ticks to keep the modulo math clean
		// use the lowest common multiple of all periods above
		if (ticks >= 50) { // all temps sampled at period of 50 ms (20 hz)
			ticks = 0;
		}
    }
}

