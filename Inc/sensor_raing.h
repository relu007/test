// ==========================================================================
// sensor_raing.h
// (c) 2022, Aurel Dumitru
//
// Description:
// Optical rain gauge sensor operations
// =========================================================================

#ifndef SENSOR_RAING_H
#define SENSOR_RAING_H

// Includes
#include "stm32h7xx_hal.h"

// Defines
#define	SENSOR_RAING_NOT_CHECKED	-1
#define	SENSOR_RAING_NOTPRESENT		 0
#define	SENSOR_RAING_PRESENT		 1

// Operations
extern void		Sensor_Raing_Init(void);
extern void		Sensor_Raing_InitServer(void);
extern char*	Sensor_Raing_GetValues(void);
extern int32_t	Sensor_Raing_GetStatus(void);
extern void		Sensor_Raing_StartMeasurement(void);
extern uint32_t Sensor_Raing_GetMeasurementStatus(void);

#endif
