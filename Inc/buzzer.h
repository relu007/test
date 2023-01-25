// ==========================================================================
// buzzer.h
// (c) 2022, Aurel Dumitru
//
// Description:
// Buzzer operations
// =========================================================================


#ifndef BUZZER_H
#define BUZZER_H

#include "stm32h7xx_hal.h"

extern void		Buzzer_MakeBeep(uint32_t Frequency, uint32_t Duration);

#endif
