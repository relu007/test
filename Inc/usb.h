// ==========================================================================
//usb.h
// (c) 2020, Aurel Dumitru
//
// Description:
// USB functionality
// =========================================================================


#ifndef USB_H
#define USB_H

#include "stm32h7xx_hal.h"

extern void Usb_InitialCheckTest(void);
extern void Usb_SetupStage(void);
extern void Usb_GetCmd(uint8_t* Buf, uint32_t Len);

#endif
