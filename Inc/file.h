// ==========================================================================
// file.h
// (c) 2020, Aurel Dumitru
//
// Description:
// FAT32 file operations
// =========================================================================

#ifndef FILE_H
#define FILE_H

// Includes
#include "stm32h7xx_hal.h"
#include "ff.h"

// Operations
extern void     File_Init(void);
extern void 	File_FormatMicroSd(void);
extern void 	File_PrepareSdPowerDown(void);

extern uint32_t File_ReadFile(const char* FileName, char* Dest);
extern void 	File_WriteFile(const char* FileName, const uint8_t* Src, uint32_t Size);
extern void		File_WriteFileAppend(const char* FileName, const uint8_t* Src, uint32_t Size);
extern void		File_DeleteFile(const char* FileName);

extern uint32_t File_ReadFileNwm(const char* FileName, char* Dest);
extern void		File_WriteFileNwm(const char* FileName, const char* Src, uint32_t Size);
extern void		File_WriteAppendFileNwm(const char* FileName, const char* Src);

extern FIL*		File_OpenReadFileFtp(const char* FileName);
extern uint32_t File_ReadFileFtp(FIL* File, uint8_t* Dest, uint32_t BufferSize);
extern void     File_CloseFileFtp(FIL* File);

extern void     File_SaveJpg(const char* PictureName, const uint8_t* Src, uint32_t Size);
extern void     File_SaveText(const char* TextName);

#endif
