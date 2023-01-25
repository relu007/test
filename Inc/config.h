//	==========================================================================
//	config.h
//	(c) 2020, Aurel Dumitru
//
//	Description:
//  Configuration header
//	==========================================================================

#ifndef CONFIG_H
#define CONFIG_H

// Includes
#include "stm32h7xx_hal.h"

struct Config_TimeEventStruct
{	uint8_t		ModemReq;
	uint8_t		SensorsReq;
	uint8_t		PheromoneFlapReq;
	uint32_t	RgbLedColor;
};

extern struct 	Config_TimeEventStruct Config_TimeEventsTable[24];
extern uint8_t  Config_NumberOfActiveInsects;
extern uint16_t Config_ActiveInsectsIdx[];
extern char		Config_PhoneNumberSms[];
extern char		Config_AllStatString[];

extern void		Config_ParseConfigFile(void);
extern uint32_t Config_ParseConfigFileImmediateActions(char* ConfigTxt);

#endif
