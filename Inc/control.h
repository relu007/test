// ==========================================================================
// control.h
// (c) 2020, Aurel Dumitru
//
// Description:
// Control operations
// =========================================================================

#ifndef CONTROL_H
#define CONTROL_H

// Includes
#include "stm32h7xx_hal.h"

// Defines
#define CONTROL_REQ_INSECT_DETECTED		1
#define CONTROL_REQ_MODEM_START			2
#define CONTROL_REQ_EVENT_PROCESS		3

// Operations
extern void		Control_Start(void);
extern void		Control_Server(void);
extern void		Control_SetRequest(uint8_t Request);
extern uint8_t	Control_IsIdle(void);
extern void 	Control_UpdateClockOnGps(int32_t TimeShift);
extern void		Control_SetTimeOnNitz(void);

#endif
