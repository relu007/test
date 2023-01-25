// ==========================================================================
// rgbled.h
// (c) 2020, Aurel Dumitru
//
// Description:
// RgbLED operations implementation
// =========================================================================


#ifndef RGBLED_H
#define RGBLED_H

#include "stm32h7xx_hal.h"

extern void		Rgbled_Init(void);
extern void		Rgbled_SetColor(uint16_t BrightnesPercentage, uint32_t RGB);
extern uint32_t	Rgbled_GetStatus(void);

#endif
