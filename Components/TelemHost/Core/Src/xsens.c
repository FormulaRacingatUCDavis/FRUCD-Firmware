/*
 * xsens.c
 *
 *  Created on: Mar 4, 2024
 *      Author: leoja
 */


#include "serial.h"
#include "xsens/xsens_mti.h"      // Main xsens library
#include "xsens/xsens_utility.h"  // Needed for quaternion conversion function
#include "can_manager.h"
#include "serial_print.h"
#include "main.h"
//#include "udp.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

extern CAN_HandleTypeDef hcan1;

#define BUFLEN 224 // should be a multiple of 32 (each cache line is 32 bytes)

#define HI8(x) ((x>>8)&0xFF)
#define LO8(x) (x&0xFF);

// PRIVATE FUNCTION PROTOTYPES
void imu_callback(XsensEventFlag_t event, XsensEventData_t *mtdata);

#define CAN_SEND_DELAY 50
uint32_t prev_sends[21] = {0};
uint32_t time_diff = 0;


// PUBLIC FUNCTION DEFINITIONS

void Xsens_Update(UART_HandleTypeDef* h_uart){
	static bool first_run = true;
	static Serial_t serial;
	static uint8_t rx_buf[BUFLEN] __ALIGNED(32); // make sure buffer is aligned to cache lines (each line is 32 bytes)
	static xsens_interface_t imu_interface = XSENS_INTERFACE_RX(&imu_callback);

	if(first_run){
		Serial_Init(&serial, h_uart, rx_buf, BUFLEN);
		Serial_StartListening(&serial);

		// in case it doesn't start reading at the right place (doesn't work)
//		static const uint8_t goto_measurement[] =
//		{ 0xFA, 0xFF, 0x10, 0x00, 0x11 };   // Xbus: Preamble, BusID, MID=0x10, LEN=0, CS
//		Serial_SendBytes(&serial, goto_measurement, sizeof(goto_measurement), 10);

		first_run = false;

		// for some reason it doesn't seem to receive anything until I've sent a byte?
		// kinda sus somebody should look into this one
//		uint8_t bytes[1] = {1};
//		Serial_SendBytes(&serial, bytes, 1, 10);
	}

	uint32_t b = Serial_BytesAvailable(&serial);
	if (b > 0) SCB_InvalidateDCache_by_Addr((uint32_t*)rx_buf, b);
	for(uint32_t i = 0; i < b; i++){
//		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);

		 xsens_mti_parse(&imu_interface, Serial_GetByte(&serial));
//		 print(rx_buf);

	}
}



// PRIVATE FUNCTION DEFINITIONS


// Called when the library decoded an inbound packet
//   - If the packet was an MData2 frame (which contains packed motion data)
//   - the callback is called once for each sub-field
void imu_callback(XsensEventFlag_t event, XsensEventData_t *mtdata)
{
    // The library provides a pointer to the a union containing decoded data
    // Use XsensEventFlag_t to determine what kind of packet arrived,
    // then copy data from the union as needed.

    // union
    // {
    //     uint8_t u1;
    //     uint16_t u2;
    //     uint32_t u4;
    //     float    f4;
    //     float    f4x2[2];
    //     float    f4x3[3];
    //     float    f4x4[4];
    //     float    f4x9[9];
    // } data;

	//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);


//	MQTTMessageFormat_t mqtt_msg;




    switch( event )
    {
		case XSENS_EVT_UTC_TIME:
			if ( mtdata->type == XSENS_EVT_TYPE_NONE )
			{
				uint32_t nanoseconds = mtdata->data.utc_time.nanoseconds;
				uint16_t year = mtdata->data.utc_time.year;
				uint8_t month = mtdata->data.utc_time.month;
				uint8_t day = mtdata->data.utc_time.day;
				uint8_t hour = mtdata->data.utc_time.hour;
				uint8_t minute = mtdata->data.utc_time.minute;
				uint8_t second = mtdata->data.utc_time.second;
				uint8_t flags = mtdata->data.utc_time.flags;

				uint8_t data[8] = {0};
                data[0] = (nanoseconds >> 24) & 0xFF;
                data[1] = (nanoseconds >> 16) & 0xFF;
                data[2] = (nanoseconds >> 8)  & 0xFF;
                data[3] = (nanoseconds >> 0)  & 0xFF;

//                data[_] = HI8(year);
//                data[_] = LO8(year);
//
//                data[_] = month;
//                data[_] = day;
                data[4] = hour;
                data[5] = minute;
                data[6] = second;
                data[7] = flags;

//                sd_card_write_data(0x101, data);
//                time_diff = HAL_GetTick() - prev_sends[0];
//                if (time_diff > CAN_SEND_DELAY) {
//                	CAN_Send(&hcan1, 0x101, data, 8);
//                	prev_sends[0] = HAL_GetTick();
//                }
                CAN_Send(&hcan1, 0x101, data, 8);
			}
                    break;

                case XSENS_EVT_PACKET_COUNT:
                    if ( mtdata->type == XSENS_EVT_TYPE_U16 )
                    {
                        uint16_t packet_count = mtdata->data.u2;
                        uint8_t data[4] = {0};
                        data[0] = HI8(packet_count);
                        data[1] = LO8(packet_count);

//                        sd_card_write_data(0x102, data);
//                        time_diff = HAL_GetTick() - prev_sends[1];
//						if (time_diff > CAN_SEND_DELAY) {
//							CAN_Send(&hcan1, 0x102, data, 2);
//							prev_sends[1] = HAL_GetTick();
//						}
                        CAN_Send(&hcan1, 0x102, data, 2);
                    }
                    break;

                case XSENS_EVT_EULER:
                    if ( mtdata->type == XSENS_EVT_TYPE_FLOAT3 )
                    {
                        int16_t ang_x = (int16_t)(mtdata->data.f4x3[0] * 100);
                        int16_t ang_y = (int16_t)(mtdata->data.f4x3[1] * 100);
                        int16_t ang_z = (int16_t)(mtdata->data.f4x3[2] * 100);

                        uint8_t data[8] = {0};
                        data[0] = HI8(ang_x);
                        data[1] = LO8(ang_x);
                        data[2] = HI8(ang_y);
                        data[3] = LO8(ang_y);
                        data[4] = HI8(ang_z);
                        data[5] = LO8(ang_z);

//                        sd_card_write_data(0x106, data);
//                        time_diff = HAL_GetTick() - prev_sends[2];
//						if (time_diff > CAN_SEND_DELAY) {
//							CAN_Send(&hcan1, 0x106, data, 6);
//							prev_sends[2] = HAL_GetTick();
//						}
                        CAN_Send(&hcan1, 0x106, data, 6);
                    }
                    break;

                case XSENS_EVT_PRESSURE:
                    if (mtdata->type == XSENS_EVT_TYPE_U32 )
                    {
                        uint32_t pressure = mtdata->data.u4;
                        uint8_t data[4] = {0};
                        data[0] = (pressure >> 24) & 0xFF;
                        data[1] = (pressure >> 16) & 0xFF;
                        data[2] = (pressure >> 8)  & 0xFF;
                        data[3] = (pressure >> 0)  & 0xFF;

//                        sd_card_write_data(0x110, data);
//                        time_diff = HAL_GetTick() - prev_sends[3];
//						if (time_diff > CAN_SEND_DELAY) {
//							CAN_Send(&hcan1, 0x110, data, 4);
//							prev_sends[3] = HAL_GetTick();
//						}
                        CAN_Send(&hcan1, 0x110, data, 4);
                    }
                    break;

                case XSENS_EVT_DELTA_Q:
                    if( mtdata->type == XSENS_EVT_TYPE_FLOAT4 )
                    {
                        int16_t delta1 = (int16_t)(mtdata->data.f4x4[0] * 100);
                        int16_t delta2 = (int16_t)(mtdata->data.f4x4[1] * 100);
                        int16_t delta3 = (int16_t)(mtdata->data.f4x4[2] * 100);
                        int16_t delta4 = (int16_t)(mtdata->data.f4x4[3] * 100);

                        uint8_t data[8] = {0};
                        data[0] = HI8(delta1);
                        data[1] = LO8(delta1);
                        data[2] = HI8(delta2);
                        data[3] = LO8(delta2);
                        data[4] = HI8(delta3);
                        data[5] = LO8(delta3);
                        data[6] = HI8(delta4);
                        data[7] = LO8(delta4);

//                        sd_card_write_data(0x112, data);
//                        time_diff = HAL_GetTick() - prev_sends[4];
//						if (time_diff > CAN_SEND_DELAY) {
//							CAN_Send(&hcan1, 0x112, data, 8);
//							prev_sends[4] = HAL_GetTick();
//						}
                        CAN_Send(&hcan1, 0x112, data, 8);
                    }
                    break;

                case XSENS_EVT_LAT_LON:
                    if ( mtdata->type == XSENS_EVT_TYPE_FLOAT2 )
                    {
                        int32_t lat = (int32_t)(mtdata->data.f4x2[0] * 10000);
                        int32_t lon = (int32_t)(mtdata->data.f4x2[1] * 10000);
                        uint8_t data[8] = {0};

                        data[0] = (lat >> 24) & 0xFF;
                        data[1] = (lat >> 16) & 0xFF;
                        data[2] = (lat >> 8)  & 0xFF;
                        data[3] = (lat >> 0)  & 0xFF;
                        data[4] = (lon >> 24) & 0xFF;
                        data[5] = (lon >> 16) & 0xFF;
                        data[6] = (lon >> 8)  & 0xFF;
                        data[7] = (lon >> 0)  & 0xFF;

//                        sd_card_write_data(0x127, data);
//                        time_diff = HAL_GetTick() - prev_sends[5];
//						if (time_diff > CAN_SEND_DELAY) {
//							CAN_Send(&hcan1, 0x127, data, 8);
//							prev_sends[5] = HAL_GetTick();
//						}
                        CAN_Send(&hcan1, 0x127, data, 8);
                    }
                    break;

                case XSENS_EVT_ACCELERATION:
                    if( mtdata->type == XSENS_EVT_TYPE_FLOAT3 )
                    {
                        int16_t accX = (int16_t)(mtdata->data.f4x3[0] * 100);
                        int16_t accY = (int16_t)(mtdata->data.f4x3[1] * 100);
                        int16_t accZ = (int16_t)(mtdata->data.f4x3[2] * 100);

                        uint8_t data[6] = {0};
                        data[0] = HI8(accX);
                        data[1] = LO8(accX);
                        data[2] = HI8(accY);
                        data[3] = LO8(accY);
                        data[4] = HI8(accZ);
                        data[5] = LO8(accZ);

//                        sd_card_write_data(0x113, data);
//                        time_diff = HAL_GetTick() - prev_sends[6];
//						if (time_diff > CAN_SEND_DELAY) {
//							CAN_Send(&hcan1, 0x113, data, 6);
//							prev_sends[6] = HAL_GetTick();
//						}
                        CAN_Send(&hcan1, 0x113, data, 6);
                    }
                    break;

                case XSENS_EVT_FREE_ACCELERATION:
                    if( mtdata->type == XSENS_EVT_TYPE_FLOAT3 )
                    {
                        int16_t acc_x = (int16_t)(mtdata->data.f4x3[0] * 100);
                        int16_t acc_y = (int16_t)(mtdata->data.f4x3[1] * 100);
                        int16_t acc_z = (int16_t)(mtdata->data.f4x3[2] * 100);

                        uint8_t data[8] = {0};
                        data[0] = HI8(acc_x);
                        data[1] = LO8(acc_x);
                        data[2] = HI8(acc_y);
                        data[3] = LO8(acc_y);
                        data[4] = HI8(acc_z);
                        data[5] = LO8(acc_z);

//                        sd_card_write_data(0x114, data);
//                        time_diff = HAL_GetTick() - prev_sends[7];
//						if (time_diff > CAN_SEND_DELAY) {
//							CAN_Send(&hcan1, 0x114, data, 6);
//							prev_sends[7] = HAL_GetTick();
//						}
                        CAN_Send(&hcan1, 0x114, data, 6);
                    }
                    break;

                case XSENS_EVT_DELTA_V:
                    if( mtdata->type == XSENS_EVT_TYPE_FLOAT3 )
                    {
                        int16_t dVX = (int16_t)(mtdata->data.f4x3[0] * 100);
                        int16_t dVY = (int16_t)(mtdata->data.f4x3[1] * 100);
                        int16_t dVZ = (int16_t)(mtdata->data.f4x3[2] * 100);

                        uint8_t data[8] = {0};
                        data[0] = HI8(dVX);
                        data[1] = LO8(dVX);
                        data[2] = HI8(dVY);
                        data[3] = LO8(dVY);
                        data[4] = HI8(dVZ);
                        data[5] = LO8(dVZ);

//                        sd_card_write_data(0x111, data);
//                        time_diff = HAL_GetTick() - prev_sends[8];
//						if (time_diff > CAN_SEND_DELAY) {
//							CAN_Send(&hcan1, 0x111, data, 6);
//							prev_sends[8] = HAL_GetTick();
//						}
                        CAN_Send(&hcan1, 0x111, data, 6);
                    }
                    break;

                case XSENS_EVT_STATUS_BYTE:
                    if ( mtdata->type == XSENS_EVT_TYPE_U8 )
                    {
                        uint8_t status_byte = mtdata->data.u1;
                        uint8_t data[8] = {0};
                        data[0] = status_byte;

//                        sd_card_write_data(0x122, data);
//                        time_diff = HAL_GetTick() - prev_sends[9];
//						if (time_diff > CAN_SEND_DELAY) {
//							CAN_Send(&hcan1, 0x122, data, 1);
//							prev_sends[9] = HAL_GetTick();
//						}
                        CAN_Send(&hcan1, 0x122, data, 1);
                    }
                    break;

                case XSENS_EVT_STATUS_WORD:
                    if ( mtdata->type == XSENS_EVT_TYPE_U32 )
                    {
                        uint32_t status_word = mtdata->data.u4;
                        uint8_t data[8] = {0};
                        data[0] = (status_word >> 24) & 0xFF;
                        data[1] = (status_word >> 16) & 0xFF;
                        data[2] = (status_word >> 8)  & 0xFF;
                        data[3] = (status_word >> 0)  & 0xFF;

//                        sd_card_write_data(0x123, data);
//                        time_diff = HAL_GetTick() - prev_sends[10];
//						if (time_diff > CAN_SEND_DELAY) {
//							CAN_Send(&hcan1, 0x123, data, 4);
//							prev_sends[10] = HAL_GetTick();
//						}
                        CAN_Send(&hcan1, 0x123, data, 4);
                    }
                    break;

                case XSENS_EVT_DEVICE_ID:
                    if ( mtdata->type == XSENS_EVT_TYPE_U32 )
                    {
                        uint32_t dev_id = mtdata->data.u4;
                        uint8_t data[8] = {0};
                        data[0] = (dev_id >> 24) & 0xFF;
                        data[1] = (dev_id >> 16) & 0xFF;
                        data[2] = (dev_id >> 8)  & 0xFF;
                        data[3] = (dev_id >> 0)  & 0xFF;

//                        sd_card_write_data(0x124, data);
//                        time_diff = HAL_GetTick() - prev_sends[11];
//						if (time_diff > CAN_SEND_DELAY) {
//							CAN_Send(&hcan1, 0x124, data, 4);
//							prev_sends[11] = HAL_GetTick();
//						}
                        CAN_Send(&hcan1, 0x124, data, 4);
                    }
                    break;

                case XSENS_EVT_LOCATION_ID:
                    if ( mtdata->type == XSENS_EVT_TYPE_U16 )
                    {
                        uint16_t location = mtdata->data.u2;
                        uint8_t data[8] = {0};
                        data[0] = HI8(location);
                        data[1] = LO8(location);

//                        sd_card_write_data(0x125, data);
//                        time_diff = HAL_GetTick() - prev_sends[12];
//						if (time_diff > CAN_SEND_DELAY) {
//							CAN_Send(&hcan1, 0x125, data, 2);
//							prev_sends[12] = HAL_GetTick();
//						}
                        CAN_Send(&hcan1, 0x125, data, 2);
                    }
                    break;

                case XSENS_EVT_POSITION_ECEF:
                    if( mtdata->type == XSENS_EVT_TYPE_FLOAT3 )
                    {
                        int16_t ecef_x = (int16_t)(mtdata->data.f4x3[0] * 100);
                        int16_t ecef_y = (int16_t)(mtdata->data.f4x3[1] * 100);
                        int16_t ecef_z = (int16_t)(mtdata->data.f4x3[2] * 100);

                        uint8_t data[8] = {0};
                        data[0] = HI8(ecef_x);
                        data[1] = LO8(ecef_x);
                        data[2] = HI8(ecef_y);
                        data[3] = LO8(ecef_y);
                        data[4] = HI8(ecef_z);
                        data[5] = LO8(ecef_z);

//                        sd_card_write_data(0x987, data); // ID is weird but we dont use position ECEF anyway
//                        time_diff = HAL_GetTick() - prev_sends[13];
//						if (time_diff > CAN_SEND_DELAY) {
//							CAN_Send(&hcan1, 0x987, data, 6);
//							prev_sends[13] = HAL_GetTick();
//						}
                        CAN_Send(&hcan1, 0x987, data, 6);
                    }
                    break;

                case XSENS_EVT_ACCELERATION_HR:
                    if( mtdata->type == XSENS_EVT_TYPE_FLOAT3 )
                    {
                        int16_t accX = (int16_t)(mtdata->data.f4x3[0] * 100);
                        int16_t accY = (int16_t)(mtdata->data.f4x3[1] * 100);
                        int16_t accZ = (int16_t)(mtdata->data.f4x3[2] * 100);

                        uint8_t data[8] = {0};
                        data[0] = HI8(accX);
                        data[1] = LO8(accX);
                        data[2] = HI8(accY);
                        data[3] = LO8(accY);
                        data[4] = HI8(accZ);
                        data[5] = LO8(accZ);

//                        sd_card_write_data(0x115, data);
//                        time_diff = HAL_GetTick() - prev_sends[14];
//						if (time_diff > CAN_SEND_DELAY) {
//							CAN_Send(&hcan1, 0x115, data, 6);
//							prev_sends[14] = HAL_GetTick();
//						}
                        CAN_Send(&hcan1, 0x115, data, 6);
                    }
                    break;

                case XSENS_EVT_RATE_OF_TURN:
                    if( mtdata->type == XSENS_EVT_TYPE_FLOAT3 )
                    {
                        int16_t gyrX = (int16_t)(mtdata->data.f4x3[0] * 100);
                        int16_t gyrY = (int16_t)(mtdata->data.f4x3[1] * 100);
                        int16_t gyrZ = (int16_t)(mtdata->data.f4x3[2] * 100);

                        uint8_t data[8] = {0};
                        data[0] = HI8(gyrX);
                        data[1] = LO8(gyrX);
                        data[2] = HI8(gyrY);
                        data[3] = LO8(gyrY);
                        data[4] = HI8(gyrZ);
                        data[5] = LO8(gyrZ);

//                        sd_card_write_data(0x116, data);
//                        time_diff = HAL_GetTick() - prev_sends[15];
//						if (time_diff > CAN_SEND_DELAY) {
//							CAN_Send(&hcan1, 0x116, data, 6);
//							prev_sends[15] = HAL_GetTick();
//						}
                        CAN_Send(&hcan1, 0x116, data, 6);
                    }
                    break;

                case XSENS_EVT_RATE_OF_TURN_HR:
                    if( mtdata->type == XSENS_EVT_TYPE_FLOAT3 )
                    {
                        int16_t gyrX = (int16_t)(mtdata->data.f4x3[0] * 100);
                        int16_t gyrY = (int16_t)(mtdata->data.f4x3[1] * 100);
                        int16_t gyrZ = (int16_t)(mtdata->data.f4x3[2] * 100);

                        uint8_t data[8] = {0};
                        data[0] = HI8(gyrX);
                        data[1] = LO8(gyrX);
                        data[2] = HI8(gyrY);
                        data[3] = LO8(gyrY);
                        data[4] = HI8(gyrZ);
                        data[5] = LO8(gyrZ);

//                        sd_card_write_data(0x117, data);
//                        time_diff = HAL_GetTick() - prev_sends[16];
//						if (time_diff > CAN_SEND_DELAY) {
//							CAN_Send(&hcan1, 0x117, data, 6);
//							prev_sends[16] = HAL_GetTick();
//						}
                        CAN_Send(&hcan1, 0x117, data, 6);
                    }
                    break;

                case XSENS_EVT_GNSS_PVT_PULSE:
                    if( mtdata->type == XSENS_EVT_TYPE_U32 )
                    {
                        uint32_t pulse = mtdata->data.u4;
                        uint8_t data[4] = {0};
                        data[0] = pulse & 0xFF;
                        data[1] = (pulse >> 8) & 0xFF;
                        data[2] = (pulse >> 16) & 0xFF;
                        data[3] = (pulse >> 24) & 0xFF;

//                        sd_card_write_data(0x118, data);
//                        time_diff = HAL_GetTick() - prev_sends[17];
//						if (time_diff > CAN_SEND_DELAY) {
//							CAN_Send(&hcan1, 0x118, data, 4);
//							prev_sends[17] = HAL_GetTick();
//						}
                        CAN_Send(&hcan1, 0x118, data, 4);
                    }
                    break;

                case XSENS_EVT_MAGNETIC:
                    if( mtdata->type == XSENS_EVT_TYPE_FLOAT3 )
                    {
                        int16_t magX = (int16_t)(mtdata->data.f4x3[0] * 100);
                        int16_t magY = (int16_t)(mtdata->data.f4x3[1] * 100);
                        int16_t magZ = (int16_t)(mtdata->data.f4x3[2] * 100);

                        uint8_t data[8] = {0};
                        data[0] = HI8(magX);
                        data[1] = LO8(magX);
                        data[2] = HI8(magY);
                        data[3] = LO8(magY);
                        data[4] = HI8(magZ);
                        data[5] = LO8(magZ);

//                        sd_card_write_data(0x121, data);
//                        time_diff = HAL_GetTick() - prev_sends[18];
//						if (time_diff > CAN_SEND_DELAY) {
//							CAN_Send(&hcan1, 0x121, data, 6);
//							prev_sends[18] = HAL_GetTick();
//						}
                        CAN_Send(&hcan1, 0x121, data, 6);
                    }
                    break;

                case XSENS_EVT_ALTITUDE_ELLIPSOID:
                    if ( mtdata->type == XSENS_EVT_TYPE_FLOAT )
                    {
                        int16_t alt = (int16_t)(mtdata->data.f4 * 100);
                        uint8_t data[8] = {0};
                        data[0] = HI8(alt);
                        data[1] = LO8(alt);

//                        sd_card_write_data(0x128, data);
//                        time_diff = HAL_GetTick() - prev_sends[19];
//						if (time_diff > CAN_SEND_DELAY) {
//							CAN_Send(&hcan1, 0x128, data, 2);
//							prev_sends[19] = HAL_GetTick();
//						}
                        CAN_Send(&hcan1, 0x128, data, 2);
                    }
                    break;

                case XSENS_EVT_VELOCITY_XYZ:
                    if ( mtdata->type == XSENS_EVT_TYPE_FLOAT3 )
                    {
                        int16_t vel_x = (int16_t)(mtdata->data.f4x3[0] * 100);
                        int16_t vel_y = (int16_t)(mtdata->data.f4x3[1] * 100);
                        int16_t vel_z = (int16_t)(mtdata->data.f4x3[2] * 100);
                        uint8_t data[8] = {0};
                        data[0] = HI8(vel_x);
                        data[1] = LO8(vel_x);
                        data[2] = HI8(vel_y);
                        data[3] = LO8(vel_y);
                        data[4] = HI8(vel_z);
                        data[5] = LO8(vel_z);

//                        sd_card_write_data(0x129, data);
//                        time_diff = HAL_GetTick() - prev_sends[20];
//						if (time_diff > CAN_SEND_DELAY) {
//							CAN_Send(&hcan1, 0x129, data, 6);
//							prev_sends[20] = HAL_GetTick();
//						}
                        CAN_Send(&hcan1, 0x129, data, 6);


                    }
                    break;

                default:
                    break;
    }
}



