#include "telem_node.h"
#include "adc.h"
#include "pwm.h"
#include "can_manager.h"
#include "main.h"
#include "wheel_speed.h"
#include "math.h"

#define VOLTAGE_DIVIDER_RATIO (12.0 / (12.0 + 6.04))
#define PSI_PER_KPA 0.145038
#define HI8(x) ((x>>8)&0xFF)
#define LO8(x) (x&0xFF);

#define NUM_SAMPLES_IN_AVGERAGE 5

TelemNodeLocation_t location = LOCATION_FRONT;

// HANDLE TYPE DEFS from main
extern ADC_HandleTypeDef hadc1;
extern uint16_t ADC_RES_BUFFER[3];
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;

// PRIVATE GLOBALS
WheelSpeed_t wheel_1;
WheelSpeed_t wheel_2;

//keep track of number of samples taken so far
uint8_t num_samples;


//vars used to calculate avg
uint64_t shock_angle1_avg = 0;
uint64_t shock_angle2_avg = 0;
uint64_t brake_temp_avg = 0;

uint16_t current_shock_angle1_avg = 0;
uint16_t current_shock_angle2_avg = 0;
uint16_t current_brake_temp_avg = 0;


// PRIVATE FUNCTION PROTOTYPES
uint16_t get_pres(uint16_t adc_val);
int16_t get_temp(uint16_t adc_val);
int16_t get_air_temp(uint16_t adc_val);
//uint16_t set_to_16(uint64_t adc_val);

uint8_t CAN_SEND_FLAG = 0;

void TelemNode_Init(){
	CAN_Init();

	WheelSpeed_Init(&wheel_1, &htim1);
	WheelSpeed_Init(&wheel_2, &htim2);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	//only have front & rear nodes

	if( num_samples == 0){
		//reading 1st set of samples
		shock_angle1_avg = ADC_RES_BUFFER[0];
		shock_angle2_avg = ADC_RES_BUFFER[1];
		brake_temp_avg = ADC_RES_BUFFER[2];

		++num_samples;

	}
	else if (num_samples < NUM_SAMPLES_IN_AVGERAGE){
		// calculate running average
		++num_samples;

		shock_angle1_avg = shock_angle1_avg * (num_samples-1)/num_samples + ADC_RES_BUFFER[0]/num_samples;
		shock_angle2_avg = shock_angle2_avg * (num_samples - 1)/ num_samples + ADC_RES_BUFFER[1]/num_samples;
		brake_temp_avg = brake_temp_avg * (num_samples-1)/num_samples + ADC_RES_BUFFER[2]/num_samples;

	}
	else{
		// save readings
		current_shock_angle1_avg = shock_angle1_avg;
		current_shock_angle2_avg = shock_angle2_avg;
		current_brake_temp_avg = brake_temp_avg;


		// reset running averages
		shock_angle1_avg = 0;
		shock_angle2_avg = 0;
		brake_temp_avg = 0;

		num_samples = 0;
	}
}

void TelemNode_Update()
{
	static uint8_t loop_count = 0;
	static uint8_t tx_data[8] = {0};

	if (CAN_SEND_FLAG != 1) return; // check for flag from TIM2 CH3 interrupt

	// brake temp
	// for 5/22/26 track day, only front Telem Node has brake temp, and only has 1
	if (location == LOCATION_FRONT) {
		tx_data[0] = HI8(current_brake_temp_avg);
		tx_data[1] = LO8(current_brake_temp_avg);
		tx_data[2] = 0;
		tx_data[3] = 0;
		CAN_Send(BRAKE_TEMP, tx_data, 2);
	}

	tx_data[0] = HI8(current_shock_angle1_avg);
	tx_data[1] = LO8(current_shock_angle1_avg);
	tx_data[2] = HI8(current_shock_angle2_avg);
	tx_data[3] = LO8(current_shock_angle2_avg);
	if (location == LOCATION_FRONT) {
		CAN_Send(SHOCK_ANGLES_FRONT, tx_data, 4);
	} else {
		CAN_Send(SHOCK_ANGLES_REAR, tx_data, 4);
	}


	uint32_t wheel_speed_1 = WheelSpeed_GetCPS(&wheel_1);
	uint32_t wheel_speed_2 = WheelSpeed_GetCPS(&wheel_2);
	// only sending bottom 16 bits of wheel speed, shouldnt need all 32 bits
	tx_data[0] = HI8(wheel_speed_1);
	tx_data[1] = LO8(wheel_speed_1);
	tx_data[2] = HI8(wheel_speed_2);
	tx_data[3] = LO8(wheel_speed_2);
	if (location == LOCATION_FRONT) {
		CAN_Send(WHEEL_SPEEDS_FRONT, tx_data, 4);
	} else {
		CAN_Send(WHEEL_SPEEDS_REAR, tx_data, 4);
	}

	CAN_SEND_FLAG = 0; // reset the flag

}

//uint16_t set_to_16(uint64_t adc_val){
//	return (uint16_t)adc_val;
//}

// no longer used since wheel speed is calculated with timers, not interrupts
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
//	switch(GPIO_Pin){
//		case WHEEL_SPEED1_Pin:
//			wheel_1.count++;
//			break;
//		case WHEEL_SPEED2_Pin:
//			wheel_2.count++;
//			break;
//	}
//}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) // TIM3 CH1 is for CAN
        {
        	// use the current timer count to calculate the next time to have this channel interrupt
            uint16_t current_count = __HAL_TIM_GET_COUNTER(htim);

            // see config.h for explanation of CAN delay
            __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, current_count + CAN_DELAY_TICKS);

           // this code runs at the frequency set above
           if (CAN_SEND_FLAG == 0) {
        	   CAN_SEND_FLAG = 1; // set the flag to signal that we should send CAN
           }
        }
    }
}



