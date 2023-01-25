// ==========================================================================
//flash.h
// (c) 2020, Aurel Dumitru
//
// Description:
// Flash dual boot implementation
// =========================================================================


#ifndef FLASH_H
#define FLASH_H

#include "stm32h7xx_hal.h"

extern void    Flash_Init(void);
extern uint8_t Flash_EraseUnusedBank(void);
extern uint8_t Flash_ProgrammUnusedBank(uint32_t Src, uint32_t Size);
extern uint8_t Flash_GetCurrentBank(void);
extern void    Flash_ChangeBootBank(void);

#endif
