// ==========================================================================
//devstatus.h
// (c) 2020, Aurel Dumitru
//
// Description:
// Device status functionality
// =========================================================================


#ifndef DEVSTATUS_H
#define DEVSTATUS_H

#include "stm32h7xx_hal.h"

extern uint32_t DevStatus[];
extern uint32_t DevStatus_InitPhase;

// Indexes
#define DEVSTATUS_POWERON_STATUS_IDX			 0
#define DEVSTATUS_RESET_COUNTER_IDX				 1
#define DEVSTATUS_CAMERA_STATUS_IDX				 2
#define DEVSTATUS_MOTOR1_STATUS_IDX				 3
#define DEVSTATUS_MOTOR2_STATUS_IDX				 4
#define DEVSTATUS_MOTORS_COUNTER_IDX			 5
#define DEVSTATUS_PICTURES_COUNTER_IDX		 	 6
#define DEVSTATUS_MODEM_STATUS_IDX			 	 7
#define DEVSTATUS_INFRARED_STATUS_IDX			 8
#define DEVSTATUS_MICROSD_STATUS_IDX			 9
#define DEVSTATUS_HYPERRAM_STATUS_IDX			10
#define DEVSTATUS_OSC32_STATUS_IDX				11
#define DEVSTATUS_BAT_STATUS_IDX				12
#define DEVSTATUS_TEMPHUMID_STATUS_IDX			13
#define DEVSTATUS_HIGH_RESET_CNT_STATUS_IDX		14
#define DEVSTATUS_RAIN_DETECTED_STATUS_IDX		15

#define DEVSTATUS_R0_FAULT_STATUS_IDX			20
#define DEVSTATUS_R1_FAULT_STATUS_IDX			21
#define DEVSTATUS_R2_FAULT_STATUS_IDX			22
#define DEVSTATUS_R3_FAULT_STATUS_IDX			23
#define DEVSTATUS_R12_FAULT_STATUS_IDX			24
#define DEVSTATUS_LR_FAULT_STATUS_IDX			25
#define DEVSTATUS_PC_FAULT_STATUS_IDX			26
#define DEVSTATUS_PSR_FAULT_STATUS_IDX			27

#define DEVSTATUS_INSECTS_VS_SENSORS_IDX		28
#define DEVSTATUS_TIME_SHIFT_IDX				29
#define DEVSTATUS_VALID_KEY1_IDX				30
#define DEVSTATUS_VALID_KEY2_IDX				31

#define DEVSTATUS_SENSORS_RS485_STATUS_IDX		59
#define DEVSTATUS_SENSOR_RS485_STATUS_IDX		60
#define DEVSTATUS_SENSOR_RGAUGE_STATUS_IDX		124
#define DEVSTATUS_SENSOR_ANALOG_STATUS_IDX		125
#define DEVSTATUS_SENSORS_VALID_KEY1_IDX		126
#define DEVSTATUS_SENSORS_VALID_KEY2_IDX		127

// Values
#define DEVSTATUS_POWERON_FALSE				 	0
#define DEVSTATUS_POWERON_TRUE				 	1
#define DEVSTATUS_CAMERA_OK					 	0
#define DEVSTATUS_CAMERA_COM_FAIL			 	1
#define DEVSTATUS_CAMERA_PICTURE_FAIL		 	2
#define DEVSTATUS_MOTOR_OK			 		 	0
#define DEVSTATUS_MOTOR_OPENLOAD	 		 	1
#define DEVSTATUS_MOTOR_JAMMED		 		 	2
#define DEVSTATUS_MODEM_OK					 	0
#define DEVSTATUS_MODEM_UART_FAIL			 	1
#define DEVSTATUS_MODEM_SIM_FAIL			 	2
#define DEVSTATUS_MODEM_REG_FAIL			 	3
#define DEVSTATUS_MODEM_NITZ_FAIL			 	4
#define DEVSTATUS_MODEM_FTPS_FAIL			 	5
#define DEVSTATUS_MODEM_SIGNAL_FAIL			 	6
#define DEVSTATUS_BAT_OK					 	0
#define DEVSTATUS_BAT_LOW					 	1
#define DEVSTATUS_INFRARED_OK				 	0
#define DEVSTATUS_INFRARED_LOW				 	1
#define DEVSTATUS_MICROSD_OK				 	0
#define DEVSTATUS_MICROSD_FAIL				 	1
#define DEVSTATUS_HYPERRAM_OK				 	0
#define DEVSTATUS_HYPERRAM_FAIL				 	1
#define DEVSTATUS_TEMPHUMID_OK				 	0
#define DEVSTATUS_TEMPHUMID_FAIL			 	1
#define DEVSTATUS_OSC32_OK					 	0
#define DEVSTATUS_OSC32_FAIL				 	1
#define DEVSTATUS_HIGH_RESET_CNT_OK				0
#define DEVSTATUS_HIGH_RESET_CNT_FAIL			1
#define DEVSTATUS_RAIN_DETECTED_NO				0
#define DEVSTATUS_RAIN_DETECTED_YES				1


// Init phases
#define DEVSTATUS_UNKNOWN_INIT_VALUE			0
#define DEVSTATUS_HYPERRAM_INIT_VALUE			1
#define DEVSTATUS_OSC32_INIT_VALUE				2

extern void 	DevStatus_Init(void);
extern void 	DevStatus_SetInitPhase(uint32_t InitPhase);
extern void 	DevStatus_WriteNfcStatus(void);

extern uint32_t DevStatus_GetBoardType(void);
extern void		DevStatus_SetDeviceType(uint32_t ForceSensorType);
extern uint32_t DevStatus_IsDeviceTypeInsectTrap(void);

extern void 	DevStatus_SaveStackAndReset(uint32_t* Stack);

#endif
