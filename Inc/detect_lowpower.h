// ==========================================================================
// detect_lowpower.h
// (c) 2020, Aurel Dumitru
//
// Description:
// IR detection and low power management
// =========================================================================


#ifndef DETECT_LOWPOWER_H
#define DETECT_LOWPOWER_H

#include "stm32h7xx_hal.h"

#define 		DETECT_LOW_POWER_SYS_FREQ_FACTOR   		4.375f	//280mHz/64mHz
#define 		DETECT_NORMAL_POWER_SYS_FREQ_FACTOR 	1.0f	//280mHz/280mHz

extern void		Detect_LowPower_Server(void);
extern uint32_t Detect_Inclination_Server(double FreqFactor);
extern uint32_t Detect_GetIrAverage(void);

extern void		Detect_LowPower_Enter(void);
extern void		Detect_LowPower_Exit(void);

extern void		Detect_Init(void);
#endif
