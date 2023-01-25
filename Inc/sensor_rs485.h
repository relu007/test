// ==========================================================================
// sensor_rs485.h
// (c) 2022, Aurel Dumitru
//
// Description:
// RS485 sensors operations
// =========================================================================

#ifndef SENSOR_RS485_H
#define SENSOR_RS485_H

// Includes
#include "stm32h7xx_hal.h"

// Defines
#define	SENSOR_RS485_NOT_CHECKED	-1
#define	SENSOR_RS485_NOTPRESENT		 0
#define	SENSOR_RS485_PRESENT		 1
#define SENSOR_RS485_PRESENT_SW		 2

// Operations
extern void		Sensor_RS485_Init(void);
extern void		Sensor_RS485_InitServer(void);
extern char*	Sensor_RS485_GetValues(void);
extern int32_t	Sensor_RS485_GetStatus(uint8_t Idx);
extern int32_t	Sensor_RS485_GetGlobalStatus(void);
extern void		Sensor_RS485_StartMeasurement(void);
extern uint32_t Sensor_RS485_GetMeasurementStatus(void);

#endif
