// ==========================================================================
// modem.h
// (c) 2020, Aurel Dumitru
//
// Description:
// SIM5XXX, SIM7XXX modems communication
// =========================================================================

#ifndef MODEM_H
#define MODEM_H

#include "stm32h7xx_hal.h"

#define MODEM_UPDATE_IDLE_STATUS					0
#define MODEM_UPDATE_CHECK_STATUS					1
#define MODEM_UPDATE_CNF_STATUS						2
#define MODEM_UPDATE_FIRMWARE_STATUS				3

#define MODEM_TESTSTATUS_FIRMWARE_BOOT				0
#define MODEM_TESTSTATUS_CONFIG_CMDS				1
#define MODEM_TESTSTATUS_NETWORK_REGISTERED			2
#define MODEM_TESTSTATUS_SERVER_LOGIN				3
#define MODEM_TESTSTATUS_ERROR						4


extern void 	Modem_Init(void);

extern void 	Modem_Server(void);
extern uint8_t	Modem_CommandsServerActivityStatus(void);
extern uint8_t	Modem_GetPowerServerState(void);

extern void		Modem_CheckFirmwareUpdate(void);
extern uint32_t	Modem_GetFirmwareUpdateStatus(void);
extern void		Modem_ResetFirmwareUpdateStatus(void);

extern 			uint8_t Modem_GetModemErrorStatus(void);

extern void 	Modem_SendSms(char* Src, char* PhoneNumber);
extern void		Modem_SendFileFtp(char* FileNameLocal, char* FileNameFtp, uint32_t Action);

extern void		Modem_OpenFtpConnection(void);
extern void		Modem_CloseFtpConnection(void);
extern int8_t 	Modem_GetSentFileStatus(void);

extern void		Modem_PowerDownHwModule(void);
extern void		Modem_PowerUpHwModule(void);
extern void		Modem_PowerRestartHwModule(void);

extern uint8_t  Modem_GetTestStatus(void);
extern char*    Modem_GetIccid(void);
extern char*	Modem_GetRssiBer(void);
extern int8_t   Modem_GetRssiInt(void);

extern char*	Modem_GpsResult(void);
extern uint32_t Modem_GetFtpsType(void);

#endif
