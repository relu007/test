// ==========================================================================
// utils.h
// (c) 2020, Aurel Dumitru
//
// Description:
// Utils operations
// ==========================================================================

#ifndef UTILS_H
#define UTILS_H

// Includes
#include "stm32h7xx_hal.h"

// Operations
extern void     Utils_InitMem(void);
extern uint8_t* Utils_AllocMem(uint32_t Size);
extern void 	Utils_FreeMem(uint8_t* Addr);

extern uint8_t* Utils_GetForegroundImgStatPtr(void);
extern uint8_t* Utils_GetForegroundImgPtr(void);
extern uint8_t* Utils_GetBackgroundImgStatPtr(void);
extern uint8_t* Utils_GetBackgroundImgPtr(void);
extern uint8_t* Utils_GetDemosaicImgPtr(void);
extern uint8_t* Utils_GetSparePsramPtr(void);

extern void     Utils_IntBcdToString(uint32_t IntBcd, char* String, uint8_t Length);
extern void     Utils_IntToHexString(uint32_t Number, char* String, uint8_t Length);
extern void     Utils_FtoaComplex(double F, uint8_t* Buf, uint8_t Precision, uint8_t Sign, uint8_t EndString);
extern char*	Utils_Ftoa0(double F);
extern char*	Utils_Ftoa1(double F);
extern char*	Utils_Ftoa5(double F);
extern uint8_t  Utils_CheckMemPattern(uint8_t* Src, uint8_t* Pattern);
extern char*    Utils_CopyString(const char* Src, char* Dest);
extern uint8_t  Utils_CheckSum8(uint8_t* Buffer, uint32_t Size);
extern void		Utils_ClearMem(uint8_t* Buffer, uint32_t Size);
extern void		Utils_ClearMsb(uint32_t* Val);

extern uint32_t Utils_GetMicroSecondsTime(void);
extern void 	Utils_DelayMicroseconds(uint32_t Microseconds);

extern uint32_t Utils_GetAdcBatteryVoltage(void);

extern void		Utils_EnterHyperRamDeepSleep(void);
extern void		Utils_ExitHyperRamDeepSleep(void);

extern void		Utils_SetUartBaudrate(UART_HandleTypeDef* huart, uint32_t BaudRate);

extern void __attribute__((section(".fast_code_sec"))) Utils_MdmaTransfer(MDMA_Channel_TypeDef* MdmaCh, uint32_t Src, uint32_t Dest, uint32_t Length);
extern void __attribute__((section(".fast_code_sec"))) Utils_MdmaTransferKeepSrc(MDMA_Channel_TypeDef* MdmaCh, uint32_t Dest, uint32_t Length);
extern void __attribute__((section(".fast_code_sec"))) Utils_MdmaTransferKeepDest(MDMA_Channel_TypeDef* MdmaCh, uint32_t Src, uint32_t Length);
extern void __attribute__((section(".fast_code_sec"))) Utils_MdmaTransferKeepSrcDest(MDMA_Channel_TypeDef* MdmaCh, uint32_t Length);
extern void __attribute__((section(".fast_code_sec"))) Utils_MdmaClearFlags(MDMA_Channel_TypeDef* MdmaCh);
extern void __attribute__((section(".fast_code_sec"))) Utils_MdmaPoolTransferComplete(MDMA_Channel_TypeDef* MdmaCh);



#endif
