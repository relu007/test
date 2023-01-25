// ==========================================================================
// rtc.h
// (c) 2020, Aurel Dumitru
//
// Description:
// Real time clock operations
// =========================================================================

#ifndef RTC_H
#define RTC_H

// Includes
#include "stm32h7xx_hal.h"

// Operations
extern uint32_t	Rtc_UpdateTimeDate(uint8_t* Text);
extern uint32_t	Rtc_GetCurrentHour(void);
extern void 	Rtc_GetCurrentTime(uint8_t* Hours, uint8_t* Minutes, uint8_t* Seconds);
extern void 	Rtc_GetCurrentTimeAndDate(int32_t* Time, int32_t* Year, int32_t* Month, int32_t* Day);
extern void		Rtc_CheckAndUpdateNextWakeUp(void);
extern uint32_t Rtc_GetRtcTimeSetStatus(void);

#endif
