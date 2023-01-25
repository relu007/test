// ==========================================================================
// sensor_nfc.h
// (c) 2020, Aurel Dumitru
//
// Description:
// BME280 operations
// =========================================================================

#ifndef SENSOR_NFC_H
#define SENSOR_NFC_H

// Includes
#include "stm32h7xx_hal.h"

// Operations
extern void		Sensor_Nfc_Init(void);
extern void 	Sensor_Nfc_FieldDetected(void);
extern uint8_t	Sensor_Nfc_ReadFieldDetectedFlag(void);

extern void		Sensor_Nfc_PowerUp(void);
extern void		Sensor_Nfc_PowerDown(void);

extern char*	Sensor_Nfc_GetDevSN(void);
extern char*	Sensor_Nfc_GetDevNumber(void);
extern void		Sensor_Nfc_GetFtpPass(char* Ptr);

extern void		Sensor_Nfc_UpdateLinkDiag(void);

#endif
