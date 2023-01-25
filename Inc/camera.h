// ==========================================================================
// camera.h
// (c) 2020, Aurel Dumitru
//
// Description:
// Camera operations
// =========================================================================

#ifndef CAMERA_H
#define CAMERA_H

// Includes
#include "stm32h7xx_hal.h"

extern void		Camera_Init(void);

extern void		Camera_SetWindowCamera(void);
extern void		Camera_ActivateFlash(uint32_t Activate);
extern void 	Camera_AdaptExposure(uint32_t Type);
extern void		Camera_DisableEmbeddedStatistics(void);

extern void		Camera_StartDmaGetFrame(uint32_t* Dest);
extern void		Camera_StartDmaGetFrameNoStatistics(uint32_t* Dest);
extern uint8_t	Camera_GetFrameRcvFlag(void);
extern void		Camera_EndFrameIsr(DCMI_HandleTypeDef * hdcmi);

extern void		Camera_EnterStandby();
extern void		Camera_ExitStandby();

#endif
