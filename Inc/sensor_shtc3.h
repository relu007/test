// ==========================================================================
// sensor_shtc3.h
// (c) 2020, Aurel Dumitru
//
// Description:
// SHTC3 operations
// =========================================================================

#ifndef SENSOR_SHTC3_H
#define SENSOR_SHTC3_H

// Includes
#include "stm32h7xx_hal.h"

// Operations
extern void		Sensor_Shtc3_Init(void);
extern void		Sensor_Shtc3_StartMeasurement(void);
extern uint32_t Sensor_Shtc3_GetTemperatureHumidity(int32_t* Temperature, uint32_t* Humidity);
extern void		Sensor_Shtc3_LogSensorData(void);
extern char*	Sensor_Shtc3_GetValues(void);
extern char*	Sensor_Shtc3_GetOldValues(void);			// to be removed
extern uint32_t	Sensor_Shtc3_GetMeasurementStatus(void);
extern char*	Sensor_Shtc3_GetFrostAlarm(void);

#endif
