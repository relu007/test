// ==========================================================================
// proc.h
// (c) 2021, Aurel Dumitru
//
// Description:
// Image processing operations
// =========================================================================


#ifndef PROC_H
#define PROC_H

#include "stm32h7xx_hal.h"

struct Proc_ContourExtraDataStruct
{	char	 Name[64];
	char	 ShortName[12];
	double	 HuMoment[7];
	double   XCenter;
	double	 YCenter;
	uint32_t PointsArea;
	double	 Perimeter;
	double	 Compactness;
	double	 Angle;
	double	 AngleDeg;
	double	 Width;
	double	 Length;
	uint32_t ColorRgbAvg; //RGB coded
	uint32_t ColorHAvg;
	uint32_t ColorSAvg;
	uint32_t ColorLAvg;
};

struct Proc_ContourStruct
{	uint16_t XMin;
	uint16_t YMin;
	uint16_t XSize;
	uint16_t YSize;
	uint16_t XStart;
	uint16_t YStart;
	uint32_t PointsNumber;
	uint8_t* Directions;
	struct Proc_ContourExtraDataStruct *ExtraData;
};


struct Proc_InsectPatternsStruct
{
	uint8_t Type;  //0=blob, 1=line, 2=angle, 3=zigzag
	uint8_t ColorType; // 1=darker/2=lighter/3=both
	uint8_t MinNumber; // min number
	uint8_t MaxNumber; // max number
};

struct Proc_InsectSegmentScanStruct      			// all dimensional data are defined in mm
{
	uint32_t Type;		//bit0: 0=maximum segment, 1=extreme points segment
						//bit1: 0=percentage of length, 1=absolute milimeters value
	double	 YMin;		//percentage of length / absolute value in mm
	double	 YMax;		//percentage of length / absolute value in mm
};

struct Proc_InsectModelStruct						// all dimensional data are defined in mm
{
	char	 Name[64];								// insect name
	char	 ShortName[12];							// insect name short
	uint32_t Type;   								// bug(0)/fly(1)
	double	 LengthRange[2];
	double	 WidthRange[2];
	double	 WingAngleRange[2];						// just for fly (in degrees)
	double	 WingSpanRange[2];						// just for fly (in mm)
	double	 Compactness[2];
	uint16_t ColorHueRange[2];
	uint16_t ColorSatRange[2];
	uint16_t ColorLumRange[2];
	double   HuMomentRange[7][2];
	uint32_t NumberOfPatternsInTexture; 			// 0=none (patterns detection is not used); max 4 patterns
	const struct   Proc_InsectPatternsStruct* Pattern;
	uint32_t NumberOfSegmentScans; 					// 0 = none (just Size and HuMoments are used as shape matching); 1 to 16 segments scan
	double   HeadTailMinSize; 						//define the minimum size to start of insect head or tail (in mm)
	const struct Proc_InsectSegmentScanStruct* SegmentScan;
	uint32_t (*InsectCheckSegMatch)(void);
};


struct Proc_InsectCropStruct
{
	char	Name[32];								// crop name
	uint32_t NumberOfInsects;						// number of insects in crop
	const struct Proc_InsectModelStruct* InsectModel;
};


extern int32_t  __attribute__((section(".fast_code_sec"))) Proc_BuildMaskDmzMalvarHeCutler(void);
extern void		__attribute__((section(".fast_code_sec"))) Proc_FastVectorMedianFilter(void);
extern void		__attribute__((section(".fast_code_sec"))) Proc_AdjustImageToOrigin(void);
extern void		__attribute__((section(".fast_code_sec"))) Proc_ApplyBrightness(int32_t BrightnessPercent);
extern uint32_t	__attribute__((section(".fast_code_sec"))) Proc_ParseContours(void);
extern uint32_t	__attribute__((section(".fast_code_sec"))) Proc_AnalyzeContours(void);
extern void		__attribute__((section(".fast_code_sec"))) Proc_RemoveCloseContours(void);
extern uint32_t __attribute__((section(".fast_code_sec"))) Proc_RemoveThinContoursBranches(void);
extern void 	__attribute__((section(".fast_code_sec"))) Proc_AdjustImageToValidContours(void);
extern void		__attribute__((section(".fast_code_sec"))) Proc_DrawContours(void);
extern void 	__attribute__((section(".fast_code_sec"))) Proc_AddContoursDataToImage(void);

extern uint32_t	__attribute__((section(".fast_code_sec"))) Proc_GetImgSizeX(void);
extern uint32_t	__attribute__((section(".fast_code_sec"))) Proc_GetImgSizeY(void);
extern void		__attribute__((section(".fast_code_sec"))) Proc_GetImgXMinYMin(uint32_t* XMin, uint32_t* YMin);
extern uint32_t __attribute__((section(".fast_code_sec"))) Proc_GetNumberOfContours(void);
extern struct Proc_ContourStruct* __attribute__((section(".fast_code_sec"))) Proc_GetContour(uint32_t Index);


extern uint32_t	Proc_CheckSegDiabroticaVirgifera(void);
extern uint32_t	Proc_CheckSegHelicoverpaArmigera(void);
extern uint32_t	Proc_CheckSegOstriniaNubilalis(void);
extern uint32_t	Proc_CheckSegLobesiaBotrana(void);
extern uint32_t	Proc_CheckSegHalyomorphaHalys(void);

#endif
