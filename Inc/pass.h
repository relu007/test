// ==========================================================================
// pass.h
// (c) 2021, Aurel Dumitru
//
// Description:
// =========================================================================


#ifndef PASS_H
#define PASS_H

#include "stm32h7xx_hal.h"

extern void Pass_Gen(char* DevString, uint8_t* Dest, uint8_t* Src, uint32_t Length);

#endif
