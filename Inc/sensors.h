// ==========================================================================
// sensors.h
// (c) 2022, Aurel Dumitru
//
// Description:
// Sensor operations
// =========================================================================

#ifndef SENSORS_H
#define SENSORS_H

// Includes
#include "stm32h7xx_hal.h"
#include "sensor_analog.h"
#include "sensor_raing.h"
#include "sensor_rs485.h"

// Operations
extern void		Sensors_Init(void);
extern void		Sensors_GetValues(char* Time);
extern void		Sensors_StartMeasurement(void);
extern uint32_t Sensors_MeasurementServer(void);

#endif
