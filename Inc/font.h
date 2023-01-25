// ==========================================================================
// font.h
// (c) 2020, Aurel Dumitru
//
// Description:
// Display text operation
// =========================================================================



#ifndef FONT_H
#define FONT_H

#include "stm32h7xx_hal.h"

extern void __attribute__((section(".fast_code_sec"))) Font_StoreCursor(void);
extern void __attribute__((section(".fast_code_sec"))) Font_RestoreCursor(void);
extern void __attribute__((section(".fast_code_sec"))) Font_SetCursorXY(uint32_t X, uint32_t Y);
extern void __attribute__((section(".fast_code_sec"))) Font_WriteText(char* Text, uint32_t FontColor);
extern void __attribute__((section(".fast_code_sec"))) Font_DrawLineAsChar(uint32_t LengthInChars, uint32_t FontColor);
extern void __attribute__((section(".fast_code_sec"))) Font_ClearLocalDisplay(uint32_t BgColor);
extern void __attribute__((section(".fast_code_sec"))) Font_CopyDisplay(uint32_t SizeX, uint32_t SizeY, uint8_t* Dest, uint32_t DestL);

#endif
