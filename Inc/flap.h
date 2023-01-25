// ==========================================================================
//flap.h
// (c) 2021, Aurel Dumitru
//
// Description:
// Flap operations
// =========================================================================


#ifndef FLAP_H
#define FLAP_H

#include "stm32h7xx_hal.h"

extern void 	Flap_SetIrState(uint8_t State);
extern uint32_t Flap_GetIrCurrent(void);
extern uint8_t Flap_GetTiltSensorValue(void);

#endif
