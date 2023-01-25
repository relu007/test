// ==========================================================================
//jpg.h
// (c) 2021, Aurel Dumitru
//
// Description:
// Jpg operations
// =========================================================================


#ifndef JPG_H
#define JPG_H

#include "stm32h7xx_hal.h"

extern uint8_t* Jpg_StartConversion(uint32_t SizeX, uint32_t SizeY, uint32_t* Size);

#endif
