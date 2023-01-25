// ==========================================================================
// sensor_analog.h
// (c) 2022, Aurel Dumitru
//
// Description:
// Analog sensor operations - Leaf Wetness supported
// =========================================================================

#ifndef SENSOR_ANALOG_H
#define SENSOR_ANALOG_H

// Includes
#include "stm32h7xx_hal.h"

// Defines
#define	SENSOR_ANALOG_NOT_CHECKED	-1
#define	SENSOR_ANALOG_NOTPRESENT	 0
#define	SENSOR_ANALOG_PRESENT		 1

// Operations
extern void		Sensor_Analog_Init(void);
extern void		Sensor_Analog_InitServer(void);
extern char*	Sensor_Analog_GetValues(void);
extern int32_t	Sensor_Analog_GetStatus(void);
extern void		Sensor_Analog_StartMeasurement(void);
extern uint32_t Sensor_Analog_GetMeasurementStatus(void);

#endif
