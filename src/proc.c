// ==========================================================================
// proc.c
// (c) 2020, Aurel Dumitru
//
// Description:
// Image processing
// =========================================================================

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "proc.h"
#include "main.h"
#include "rgbled.h"
#include "font.h"
#include "utils.h"
#include "devstatus.h"
#include "config.h"
#include "calibrations.h"

#define L		960
#define LF		964
#define LF2		(964*2)

#define PROC_DIR_TYPE_HPLUS		0
#define PROC_DIR_TYPE_HMINUS	4
#define PROC_DIR_TYPE_BEGIN		5
#define PROC_DIR_TYPE_END		1

#define PROC_FILL_MASK_ORIGINAL 0
#define PROC_FILL_MASK_ALL		1

struct Proc_ContourXStruct
{	uint16_t X;
	uint16_t NextX;
};


struct Proc_IntervalSegmentInfo
{
	int32_t MinPos;
	int32_t	MinSize;
	int32_t MaxPos;
	int32_t MaxSize;
	int32_t AvgSize;
};

uint8_t	 Proc_ImgMaskOne[L*L/8];
uint8_t	 Proc_ImgMaskTwo[L*L/8];
uint8_t	 Proc_ImgMaskNormalized[L*L/8];
uint8_t  *Proc_ImgMask;
uint8_t  *Proc_ImgMaskSec;
int32_t  Proc_XMin, Proc_YMin, Proc_XMax, Proc_YMax;
int32_t  Proc_SizeX, Proc_SizeY;

struct   Proc_ContourStruct			 Proc_Contour[NC_MAX_VALID_CONTOURS];
struct	 Proc_ContourExtraDataStruct Proc_ContourExtraData[NC_MAX_ANALYSED_CONTOURS];
uint32_t Proc_NumberOfContours;
uint32_t Proc_NumberOfRemovedContours;
uint8_t	 Proc_ContourDirections[64*1024];
uint8_t* Proc_LastContourDirection;
uint32_t Proc_ContourXMinMax[L][2];
uint16_t Proc_ContourSegmentsY[L];
struct   Proc_ContourXStruct Proc_ContourSegmentsX[16384];
uint8_t  Proc_ContoursThumbnails[32*32*3];
uint32_t Proc_ContourFreeX;
uint32_t Proc_NormalizedMaskWidth;
uint32_t Proc_NormalizedMaskHeight;

uint32_t Proc_SmallRainDropCounter;
uint32_t Proc_LargeRainDropCounter;

int32_t  Proc_DirectionAddX[8]  = {1,  1,  0, -1, -1, -1,  0,  1};
int32_t  Proc_DirectionAddY[8]  = {0, -1, -1, -1,  0,  1,  1,  1};
double   Proc_DirectionAddXf[8] = {1.0F,  1.0F,  0.0F, -1.0F, -1.0F, -1.0F,  0.0F,  1.0F};
double   Proc_DirectionAddYf[8] = {0.0F, -1.0F, -1.0F, -1.0F,  0.0F,  1.0F,  1.0F,  1.0F};
double 	 Proc_PerimeterAdd[8]	= {1.0F, 1.41421356237F, 1.0F, 1.41421356237F, 1.0F, 1.41421356237F, 1.0F, 1.41421356237F};


int32_t Proc_HeadSegmentXCenter;
int32_t Proc_HeadSegmentYCenter;
int32_t Proc_WingLength[2];
double  Proc_WingAngle;
struct Proc_IntervalSegmentInfo Proc_IntervalSegmentData[16];

double Temp[4];


__attribute__((section(".dtcm_data_sec"))) uint16_t Proc_BufferMdmaTwo[61*1024/2];
__attribute__((section(".dtcm_data_sec"))) uint8_t  Proc_BufferRgbLine[3*1024];
__attribute__((section(".dtcm_data_sec"))) uint16_t Proc_BufferMdmaOne[61*1024/2];
__attribute__((section(".dtcm_data_sec"))) uint8_t  Proc_BufferRainDetectionLine[1024];
__attribute__((section(".dtcm_data_sec"))) uint32_t Proc_BufferCopyRgb[2*1024/4];


///////////////////////////////////////////////////////////
// Private operations
///////////////////////////////////////////////////////////

static inline uint32_t Proc_GetMaskXY(uint32_t X, uint32_t Y)
{
	return (uint32_t)Proc_ImgMask[(Y*L + X)>>3] & (1 << (X & 0x07));
}

static inline uint32_t Proc_GetMaskLinear(uint32_t Index)
{
	return (uint32_t)Proc_ImgMask[Index>>3] & (1 << (Index & 0x07));
}

static inline uint32_t Proc_GetMaskLinearNormalized(uint32_t Index)
{
	return (uint32_t)Proc_ImgMaskNormalized[Index>>3] & (1 << (Index & 0x07));
}

static inline void		Proc_SetMaskXY(uint32_t X, uint32_t Y)
{
	Proc_ImgMaskSec[(Y*L + X)>>3] |= (1 << (X & 0x07));
}

static inline void		Proc_SetMaskLinear(uint32_t Index)
{
	Proc_ImgMaskSec[Index>>3] |= (1 << (Index & 0x07));
}

static inline void		Proc_ClearMaskXY(uint32_t X, uint32_t Y)
{
	Proc_ImgMaskSec[(Y*L + X)>>3] &= ~(1 << (X & 0x07));
}

static inline void		Proc_ClearMaskLinear(uint32_t Index)
{
	Proc_ImgMaskSec[Index>>3] &= ~(1 << (Index & 0x07));
}

void Proc_ClearMaskAll(uint8_t* Mask)
{
	uint32_t* MaskEnd  = (uint32_t*)(Mask + (L*L)/8);
	uint32_t* MaskClear = (uint32_t*)(Mask);
	while (MaskClear != MaskEnd)
		*MaskClear++ = 0x00000000;
}

void Proc_ClearMask(uint8_t* Mask)
{
	uint32_t I;
	uint8_t* Dest = Mask + (((Proc_YMin)*L + Proc_XMin)>>3);
	uint8_t* DestEnd;

	I = Proc_SizeY;
	while (I--)
	{
		DestEnd = Dest + (Proc_SizeX>>3);
		while (Dest != DestEnd)
			*Dest++ = 0x00;
		Dest += (L-Proc_SizeX)>>3;
	}
}

void Proc_CopyMaskAll(uint8_t* Mask, uint8_t* MaskSec)
{
	uint32_t* Src	 = (uint32_t*)(Mask);
	uint32_t* SrcEnd = (uint32_t*)(Mask + (L*L)/8);
	uint32_t* Dest = (uint32_t*)(MaskSec);
	while (Src != SrcEnd)
		*Dest++ = *Src++;
}


void Proc_CopyMask(uint8_t* Mask, uint8_t* MaskSec)
{
	uint32_t I;
	uint8_t* Src  = Mask    + (((Proc_YMin)*L + Proc_XMin)>>3);
	uint8_t* Dest = MaskSec + (((Proc_YMin)*L + Proc_XMin)>>3);
	uint8_t* DestEnd;

	I = Proc_SizeY;
	while (I--)
	{
		DestEnd = Dest + (Proc_SizeX>>3);
		while (Dest != DestEnd)
			*Dest++ = *Src++;
		Src  += (L-Proc_SizeX)>>3;
		Dest += (L-Proc_SizeX)>>3;
	}
}

void Proc_ClearMaskMargins(uint8_t* Mask)
{
	uint8_t* Src;
	uint32_t I;
	// Top
	Src = Mask;
	I = L/8;
	while (I--)
		*Src++ = 0x00;
	//Bottom
	Src = Mask + (L-1)*L/8;
	I = L/8;
	while (I--)
		*Src++ = 0x00;
	//Left
	Src = Mask + (L/8);
	I = L-2;
	while (I--)
	{
		*Src &= 0xFE;
		Src += L/8;
	}
	//Right
	Src = Mask + ((2*L-1)/8);
	I = L-2;
	while (I--)
	{
		*Src &= 0x7F;
		Src += L/8;
	}
}


void __attribute__((section(".fast_code_sec"))) Proc_AddNormMaskOverPicture(uint32_t Width, uint32_t Height)
{
	uint32_t X,Y;
	uint8_t* Dest;
	uint32_t Index;

	for (Y=0; Y<Height; Y++)
	{
		Dest = (uint8_t*)Utils_GetDemosaicImgPtr() + (Y*Proc_SizeX)*3;
		Index = Y*Width;
		for (X=0; X<Width; X++)
		{
			if (Proc_GetMaskLinearNormalized(Index++))
			{
				*Dest++ = 0xD0;
				*Dest++ = 0xD0;
				*Dest++ = 0xD0;
			}
			else
				Dest += 3;
		}
	}
}


void __attribute__((section(".fast_code_sec"))) Proc_MaskErosionSquare3x3(void)
{
	uint32_t I, J;
	uint32_t Index = L+1;

	Proc_ClearMaskAll(Proc_ImgMaskSec);
	J = L-2;
    while (J--)
    {
    	I = L-2;
     	while (I--)
    	{
   			if (Proc_GetMaskLinear(Index-1) &&
   				Proc_GetMaskLinear(Index+1) &&
				Proc_GetMaskLinear(Index-L-1) &&
				Proc_GetMaskLinear(Index-L) &&
				Proc_GetMaskLinear(Index-L+1) &&
				Proc_GetMaskLinear(Index+L-1) &&
				Proc_GetMaskLinear(Index+L) &&
				Proc_GetMaskLinear(Index+L+1))
   					Proc_SetMaskLinear(Index);
   			Index++;
    	}
     	Index += 2;
    }
	uint8_t* Src8  	= Proc_ImgMask;
	Proc_ImgMask 	= Proc_ImgMaskSec;
	Proc_ImgMaskSec = Src8;
}


void __attribute__((section(".fast_code_sec"))) Proc_MaskDilationSquare3x3(void)
{
	uint32_t I, J;
	uint32_t Index = L+1;

	Proc_ClearMaskAll(Proc_ImgMaskSec);
	J = L-2;
    while (J--)
    {
    	I = L-2;
     	while (I--)
    	{
    		if (Proc_GetMaskLinear(Index))
    		{
    			Proc_SetMaskLinear(Index);
    			Proc_SetMaskLinear(Index-1);
       			Proc_SetMaskLinear(Index+1);
       			Proc_SetMaskLinear(Index-L-1);
       			Proc_SetMaskLinear(Index-L);
       			Proc_SetMaskLinear(Index-L+1);
       			Proc_SetMaskLinear(Index+L-1);
       			Proc_SetMaskLinear(Index+L);
       			Proc_SetMaskLinear(Index+L+1);
    		}
     		Index++;
    	}
     	Index += 2;
    }
    uint8_t* Src8	= Proc_ImgMask;
	Proc_ImgMask 	= Proc_ImgMaskSec;
	Proc_ImgMaskSec = Src8;
}

void __attribute__((section(".fast_code_sec"))) Proc_MaskRemoveSingularPoints(void)
{
	uint32_t I, J;
	uint32_t Index = L+1;

	Proc_ClearMaskAll(Proc_ImgMaskSec);
	J = L-2;
    while (J--)
    {
    	I = L-2;
     	while (I--)
    	{
     		if (Proc_GetMaskLinear(Index))
     		{
     			if (Proc_GetMaskLinear(Index-1) ||
     				Proc_GetMaskLinear(Index+1) ||
					Proc_GetMaskLinear(Index-L-1) ||
					Proc_GetMaskLinear(Index-L) ||
					Proc_GetMaskLinear(Index-L+1) ||
					Proc_GetMaskLinear(Index+L-1) ||
					Proc_GetMaskLinear(Index+L) ||
					Proc_GetMaskLinear(Index+L+1))
   					Proc_SetMaskLinear(Index);
     		}
   			Index++;
    	}
     	Index += 2;
    }
	uint8_t* Src8  	= Proc_ImgMask;
	Proc_ImgMask 	= Proc_ImgMaskSec;
	Proc_ImgMaskSec = Src8;
}


void __attribute__((section(".fast_code_sec"))) Proc_MaskFillSmallGaps(void)
{
	uint32_t X, Y;
	uint32_t Index;
	uint32_t Sum;
	uint32_t AddY = L+2-Proc_SizeX;

	for (Y=(Proc_SizeY-2), Index=((Proc_YMin+1)*L+Proc_XMin+1); Y>0; Y--, Index+=AddY)
		for (X=(Proc_SizeX-2); X>0; X--, Index++)
			if (Proc_GetMaskLinear(Index) == 0)
			{
				Sum = 0;
				if (Proc_GetMaskLinear(Index-1))		Sum++;
				if (Proc_GetMaskLinear(Index+1))		Sum++;
				if (Proc_GetMaskLinear(Index-L-1))		Sum++;
				if (Proc_GetMaskLinear(Index-L))		Sum++;
				if (Sum == 0) 							continue;
				if (Proc_GetMaskLinear(Index-L+1))		Sum++;
				if (Proc_GetMaskLinear(Index+L-1))		Sum++;
				if (Proc_GetMaskLinear(Index+L))		Sum++;
				if (Proc_GetMaskLinear(Index+L+1))		Sum++;
				if (Sum >= 5)							Proc_ImgMask[Index>>3] |= (1 << (Index & 0x07));
			}
}


void __attribute__((section(".fast_code_sec"))) Proc_InitRainDropReflection(void)
{
	uint32_t * Addr = (uint32_t*)Proc_BufferRainDetectionLine;
	uint32_t * AddrEnd = Addr + (L/4);
	while (Addr < AddrEnd)
		*Addr++ = 0x00000000;
	Proc_SmallRainDropCounter = 0;
	Proc_LargeRainDropCounter = 0;
}

void __attribute__((section(".fast_code_sec"))) Proc_CheckRainDropReflection(uint32_t Line)
{
	int32_t I, J;
	int32_t R,  G,  B;
	int32_t FracGR, FracBR;
	uint8_t* Src;
	uint8_t* SrcS;
	uint8_t* SrcTmp;
	int32_t IllegalSpot;
	int32_t PrevBright;
	int32_t YellowDetected;


	if ((Line >= ((L-NC_RAIN_DROP_WINDOW_Y)/2)) && (Line < ((L+NC_RAIN_DROP_WINDOW_Y)/2)))
	{
		PrevBright = 0;
		YellowDetected = -1;
		SrcS = Proc_BufferRainDetectionLine + (L-NC_RAIN_DROP_WINDOW_X)/2;
		for (I=(L-NC_RAIN_DROP_WINDOW_X)/2; I<((L+NC_RAIN_DROP_WINDOW_X)/2);I++)
		{
			Src = Proc_BufferRgbLine + 3*I;
			B = Src[0];
			G = Src[1];
			R = Src[2];
			FracGR = (G<<8)/R;
			FracBR = (B<<8)/R;

			if ((R>140) && (R<245) && (FracGR > 200) && (FracGR < 260) && (FracBR < 128))
				YellowDetected = 3;

			if ((R>=220) && (G>=220) && (B>=120))
			{
				// Check left right colors
				IllegalSpot = 0;

				B = *(Src-3);
				G = *(Src-2);
				R = *(Src-1);
				FracGR = (G<<8)/R;
				FracBR = (B<<8)/R;
				if ((R<140) || (FracGR < 200) || (FracGR > 260) || (FracBR > 256))
					IllegalSpot = 1;
				else
				{
					B = *(Src+3);
					G = *(Src+4);
					R = *(Src+5);
					FracGR = (G<<8)/R;
					FracBR = (B<<8)/R;
					if ((R<140) || (FracGR < 200) || (FracGR > 260) || (FracBR > 256))
						IllegalSpot = 1;
				}

				if (IllegalSpot || (YellowDetected<0))
				{
					*SrcS = 0;
					SrcTmp = SrcS;
					SrcS--;
					for (J=I-1; J>=(L-NC_RAIN_DROP_WINDOW_X)/2; J--)
					{
						if (*SrcS)
							*SrcS = 0;
						else
							break;
						SrcS--;
					}
					SrcS = SrcTmp;
					SrcS++;
 					for (J=I+1; J<((L+NC_RAIN_DROP_WINDOW_X)/2); J++)
					{
						if (*SrcS)
							*SrcS = 0;
						else
							break;
						SrcS++;
					}
					I = J-1;
					PrevBright = 0;
					YellowDetected = -1;
				}
				else
				{
					PrevBright = (*SrcS) + 1;
					if (PrevBright < *(SrcS-1))
							PrevBright = *(SrcS-1);
					*SrcS++ = PrevBright;
					PrevBright = L;
				}
			}
			else
			{
				YellowDetected--;
				if (*SrcS > 0)
				{
					if (PrevBright < *SrcS)
						PrevBright = *SrcS;
					*SrcS = 0;
				}
				else
				{
					if ((PrevBright > 0) && (PrevBright < 8))
					{
						if (PrevBright > 2)
							Proc_LargeRainDropCounter++;
						else
							Proc_SmallRainDropCounter++;
					}
					PrevBright = 0;
				}
				SrcS++;
			}
			Src += 3;
		}
	}
}

uint32_t __attribute__((section(".fast_code_sec"))) Proc_ResultsRainDropReflection(void)
{
	uint32_t RainDetected;
	if ((RainDetected = ((Proc_LargeRainDropCounter + Proc_SmallRainDropCounter) >= NC_THRESHOLD_RAIN_DROP_COUNTER)))
		if (C_ActivateBlueLedOnRain)
			Rgbled_SetColor(100, 0x0000FF);
	return RainDetected;
}


///////////////////////////////////////////////////////////
// Public operations
///////////////////////////////////////////////////////////


uint32_t __attribute__((section(".fast_code_sec"))) Proc_GetImgSizeX(void)
{
	return Proc_SizeX;
}
uint32_t __attribute__((section(".fast_code_sec"))) Proc_GetImgSizeY(void)
{
	return Proc_SizeY;
}
void __attribute__((section(".fast_code_sec"))) Proc_GetImgXMinYMin(uint32_t* XMin, uint32_t* YMin)
{
	*XMin = Proc_XMin;
	*YMin = Proc_YMin;
}

uint32_t __attribute__((section(".fast_code_sec"))) Proc_GetNumberOfContours(void)
{
	return Proc_NumberOfContours;
}
struct Proc_ContourStruct* __attribute__((section(".fast_code_sec"))) Proc_GetContour(uint32_t Index)
{
	return &Proc_Contour[Index];
}

int32_t __attribute__((section(".fast_code_sec"))) Proc_BuildMaskDmzMalvarHeCutler(void)
{
	int32_t Rf, Gf, Bf;
	int32_t Rb, Gb, Bb;
	int32_t Delta;
	uint32_t Rratio, Gratio, Bratio;
	uint32_t Fract, LowTh, HighTh;

	uint32_t	Index = 0;
	uint8_t*	Dest;
	int32_t		I, J, K;
	int32_t		X, Y;
	uint32_t    Z = 28;

	uint32_t SrcDmaFg = (uint32_t)Utils_GetForegroundImgPtr();
	uint32_t SrcDmaBg = (uint32_t)Utils_GetBackgroundImgPtr();
	uint32_t DestRGB  = (uint32_t)Utils_GetDemosaicImgPtr();

	Utils_MdmaClearFlags(MDMA_Channel1);
	Utils_MdmaTransfer(MDMA_Channel0, SrcDmaFg, (uint32_t)Proc_BufferMdmaOne, 32*LF2);
	Utils_MdmaTransfer(MDMA_Channel1, SrcDmaBg, (uint32_t)Proc_BufferMdmaTwo, 32*LF2);

	// Clear the whole mask
	Proc_ImgMaskSec = Proc_ImgMaskOne;
	Proc_ClearMaskAll(Proc_ImgMaskSec);
	// Initialization rain drop reflection detection
	Proc_InitRainDropReflection();

	uint16_t* ImgFg = Proc_BufferMdmaOne + LF2 + 2;
	uint16_t* ImgBg = Proc_BufferMdmaTwo + LF2 + 2;

	for (K=0; K<35; K++)
	{
		Utils_MdmaPoolTransferComplete(MDMA_Channel1);
		for (J=0; J<Z; J++)
		{
			Dest = Proc_BufferRgbLine;
			for (I=0; I<L; I++)
			{
				if (I&1)
				{
					// Blue ROW odd
					if (J&1)
					{
						Bf  = (((int32_t)(*(ImgFg)))*5 +
							 (((int32_t)(*(ImgFg-1))) + ((int32_t)(*(ImgFg+1))))*4 +
							 (((int32_t)(*(ImgFg-LF2))) + ((int32_t)(*(ImgFg+LF2))))/2 -
							 ((int32_t)(*(ImgFg-LF-1))) - ((int32_t)(*(ImgFg-LF+1))) -
							 ((int32_t)(*(ImgFg-2))) - ((int32_t)(*(ImgFg+2))) -
							 ((int32_t)(*(ImgFg+LF-1))) - ((int32_t)(*(ImgFg+LF+1)))) >> 3;
						if (Bf <= 0) Bf = 1;
						Bf = __USAT(Bf, 12);
						Gf = *(ImgFg);
						if (Gf == 0) Gf++;
						Rf  = (((int32_t)(*(ImgFg)))*5 +
							 (((int32_t)(*(ImgFg-LF))) + ((int32_t)(*(ImgFg+LF))))*4 +
							 (((int32_t)(*(ImgFg-2))) + ((int32_t)(*(ImgFg+2))))/2 -
							 ((int32_t)(*(ImgFg-LF2))) - ((int32_t)(*(ImgFg+LF2))) -
							 ((int32_t)(*(ImgFg-LF-1))) - ((int32_t)(*(ImgFg-LF+1))) -
							 ((int32_t)(*(ImgFg+LF-1))) - ((int32_t)(*(ImgFg+LF+1)))) >> 3;
						if (Rf <= 0) Rf = 1;
						Rf = __USAT(Rf, 12);

						// Correction in case much BLUE
						if (Bf > Gf)
						{
							Delta = ((Bf-Gf)*Gf)/(148*16);
							Gf += Delta;
							Gf = __USAT(Gf, 12);
							Rf -= Delta;
							if (Rf <= 0) Rf = 1;
						}
						*Dest++ = (Bf>>4);
						*Dest++ = (Gf>>4);
						*Dest++ = (Rf>>4);

						Bb  = (((int32_t)(*(ImgBg)))*5 +
							 (((int32_t)(*(ImgBg-1))) + ((int32_t)(*(ImgBg+1))))*4 +
							 (((int32_t)(*(ImgBg-LF2))) + ((int32_t)(*(ImgBg+LF2))))/2 -
							 ((int32_t)(*(ImgBg-LF-1))) - ((int32_t)(*(ImgBg-LF+1))) -
							 ((int32_t)(*(ImgBg-2))) - ((int32_t)(*(ImgBg+2))) -
							 ((int32_t)(*(ImgBg+LF-1))) - ((int32_t)(*(ImgBg+LF+1)))) >> 3;
						if (Bb <= 0) Bb = 1;
						Bb = __USAT(Bb, 12);
						Gb = *(ImgBg);
						if (Gb == 0) Gb++;
						Rb  = (((int32_t)(*(ImgBg)))*5 +
							 (((int32_t)(*(ImgBg-LF))) + ((int32_t)(*(ImgBg+LF))))*4 +
							 (((int32_t)(*(ImgBg-2))) + ((int32_t)(*(ImgBg+2))))/2 -
							 ((int32_t)(*(ImgBg-LF2))) - ((int32_t)(*(ImgBg+LF2))) -
							 ((int32_t)(*(ImgBg-LF-1))) - ((int32_t)(*(ImgBg-LF+1))) -
							 ((int32_t)(*(ImgBg+LF-1))) - ((int32_t)(*(ImgBg+LF+1)))) >> 3;
						if (Rb <= 0) Rb = 1;
						Rb = __USAT(Rb, 12);

						// Correction in case much BLUE
						if (Bb > Gb)
						{
							Delta = ((Bb-Gb)*Gb)/(148*16);
							Gb += Delta;
							Gb = __USAT(Gb, 12);
							Rb -= Delta;
							if (Rb <= 0) Rb = 1;
						}
					}
					else
					{
						// Red ROW odd
						Bf = (((int32_t)(*(ImgFg)))*12 +
							(((int32_t)(*(ImgFg-LF-1))) + ((int32_t)(*(ImgFg-LF+1))) + ((int32_t)(*(ImgFg+LF-1))) + ((int32_t)(*(ImgFg+LF+1))))*4 -
							(((int32_t)(*(ImgFg-LF2))) + ((int32_t)(*(ImgFg+LF2))) + ((int32_t)(*(ImgFg-2))) + ((int32_t)(*(ImgFg+2))))*3) >> 4;
						if (Bf <= 0) Bf = 1;
						Bf = __USAT(Bf, 12);
						Gf = (((int32_t)(*(ImgFg)))*4 +
							(((int32_t)(*(ImgFg-LF))) + ((int32_t)(*(ImgFg+LF))) + ((int32_t)(*(ImgFg-1))) + ((int32_t)(*(ImgFg+1))))*2 -
							((int32_t)(*(ImgFg-LF2))) - ((int32_t)(*(ImgFg+LF2))) -
							((int32_t)(*(ImgFg-2))) - ((int32_t)(*(ImgFg+2)))) >> 3;
						if (Gf <= 0) Gf = 1;
						Gf = __USAT(Gf, 12);
						Rf = *(ImgFg);
						if (Rf == 0) Rf++;

						// Correction in case much BLUE
						if (Bf > Gf)
						{
							Delta = ((Bf-Gf)*Gf)/(148*16);
							Gf += Delta;
							Gf = __USAT(Gf, 12);
							Rf -= Delta;
							if (Rf <= 0) Rf = 1;
						}
						*Dest++ = (Bf>>4);
						*Dest++ = (Gf>>4);
						*Dest++ = (Rf>>4);

						Bb = (((int32_t)(*(ImgBg)))*12 +
							(((int32_t)(*(ImgBg-LF-1))) + ((int32_t)(*(ImgBg-LF+1))) + ((int32_t)(*(ImgBg+LF-1))) + ((int32_t)(*(ImgBg+LF+1))))*4 -
							(((int32_t)(*(ImgBg-LF2))) + ((int32_t)(*(ImgBg+LF2))) + ((int32_t)(*(ImgBg-2))) + ((int32_t)(*(ImgBg+2))))*3) >> 4;
						if (Bb <= 0) Bb = 1;
						Bb = __USAT(Bb, 12);
						Gb = (((int32_t)(*(ImgBg)))*4 +
							(((int32_t)(*(ImgBg-LF))) + ((int32_t)(*(ImgBg+LF))) + ((int32_t)(*(ImgBg-1))) + ((int32_t)(*(ImgBg+1))))*2 -
							((int32_t)(*(ImgBg-LF2))) - ((int32_t)(*(ImgBg+LF2))) -
							((int32_t)(*(ImgBg-2))) - ((int32_t)(*(ImgBg+2)))) >> 3;
						if (Gb <= 0) Gb = 1;
						Gb = __USAT(Gb, 12);
						Rb = *(ImgBg);
						if (Rb == 0) Rb++;

						// Correction in case much BLUE
						if (Bb > Gb)
						{
							Delta = ((Bb-Gb)*Gb)/(148*16);
							Gb += Delta;
							Gb = __USAT(Gb, 12);
							Rb -= Delta;
							if (Rb <= 0) Rb = 1;
						}
					}
				}
				else
				{
					// Blue ROW even
					if (J&1)
					{
						Bf = *(ImgFg);
						if (Bf == 0) Bf++;
						Gf = (((int32_t)(*(ImgFg)))*4 +
							(((int32_t)(*(ImgFg-LF))) + ((int32_t)(*(ImgFg+LF))) + ((int32_t)(*(ImgFg-1))) + ((int32_t)(*(ImgFg+1))))*2 -
							((int32_t)(*(ImgFg-LF2))) - ((int32_t)(*(ImgFg+LF2))) - ((int32_t)(*(ImgFg-2))) - ((int32_t)(*(ImgFg+2)))) >> 3;
						if (Gf <= 0) Gf = 1;
						Gf = __USAT(Gf, 12);
						Rf = (((int32_t)(*(ImgFg)))*12 +
						    (((int32_t)(*(ImgFg-LF-1))) + ((int32_t)(*(ImgFg-LF+1))) + ((int32_t)(*(ImgFg+LF-1))) + ((int32_t)(*(ImgFg+LF+1))))*4 -
							(((int32_t)(*(ImgFg-LF2))) + ((int32_t)(*(ImgFg+LF2))) + ((int32_t)(*(ImgFg-2))) + ((int32_t)(*(ImgFg+2))))*3) >> 4;
						if (Rf <= 0) Rf = 1;
						Rf = __USAT(Rf, 12);

						// Correction in case much BLUE
						if (Bf > Gf)
						{
							Delta = ((Bf-Gf)*Gf)/(148*16);
							Gf += Delta;
							Gf = __USAT(Gf, 12);
							Rf -= Delta;
							if (Rf <= 0) Rf = 1;
						}
						*Dest++ = (Bf>>4);
						*Dest++ = (Gf>>4);
						*Dest++ = (Rf>>4);

						Bb = *(ImgBg);
						if (Bb == 0) Bb++;
						Gb = (((int32_t)(*(ImgBg)))*4 +
							(((int32_t)(*(ImgBg-LF))) + ((int32_t)(*(ImgBg+LF))) + ((int32_t)(*(ImgBg-1))) + ((int32_t)(*(ImgBg+1))))*2 -
							((int32_t)(*(ImgBg-LF2))) - ((int32_t)(*(ImgBg+LF2))) - ((int32_t)(*(ImgBg-2))) - ((int32_t)(*(ImgBg+2)))) >> 3;
						if (Gb <= 0) Gb = 1;
						Gb = __USAT(Gb, 12);
						Rb = (((int32_t)(*(ImgBg)))*12 +
						    (((int32_t)(*(ImgBg-LF-1))) + ((int32_t)(*(ImgBg-LF+1))) + ((int32_t)(*(ImgBg+LF-1))) + ((int32_t)(*(ImgBg+LF+1))))*4 -
							(((int32_t)(*(ImgBg-LF2))) + ((int32_t)(*(ImgBg+LF2))) + ((int32_t)(*(ImgBg-2))) + ((int32_t)(*(ImgBg+2))))*3) >> 4;
						if (Rb <= 0) Rb = 1;
						Rb = __USAT(Rb, 12);

						// Correction in case much BLUE
						if (Bb > Gb)
						{
							Delta = ((Bb-Gb)*Gb)/(148*16);
							Gb += Delta;
							Gb = __USAT(Gb, 12);
							Rb -= Delta;
							if (Rb <= 0) Rb = 1;
						}
					}
					else
					{
						// Red ROW even
						Bf = (((int32_t)(*(ImgFg)))*5 +
							(((int32_t)(*(ImgFg-LF))) + ((int32_t)(*(ImgFg+LF))))*4 +
							(((int32_t)(*(ImgFg-2))) + ((int32_t)(*(ImgFg+2))))/2 -
							((int32_t)(*(ImgFg-LF2))) - ((int32_t)(*(ImgFg+LF2))) -
							((int32_t)(*(ImgFg-LF-1))) - ((int32_t)(*(ImgFg-LF+1))) -
							((int32_t)(*(ImgFg+LF-1))) - ((int32_t)(*(ImgFg+LF+1)))) >> 3;
						if (Bf <= 0) Bf = 1;
						Bf = __USAT(Bf, 12);
						Gf = *(ImgFg);
						if (Gf == 0) Gf++;
						Rf = (((int32_t)(*(ImgFg)))*5 +
							(((int32_t)(*(ImgFg-1))) + ((int32_t)(*(ImgFg+1))))*4 +
							(((int32_t)(*(ImgFg-LF2))) + ((int32_t)(*(ImgFg+LF2))))/2 -
							((int32_t)(*(ImgFg-LF-1))) - ((int32_t)(*(ImgFg-LF+1))) -
							((int32_t)(*(ImgFg-2))) - ((int32_t)(*(ImgFg+2))) -
							((int32_t)(*(ImgFg+LF-1))) - ((int32_t)(*(ImgFg+LF+1)))) >> 3;
						if (Rf <= 0) Rf = 1;
						Rf = __USAT(Rf, 12);

						// Correction in case much BLUE
						if (Bf > Gf)
						{
							Delta = ((Bf-Gf)*Gf)/(148*16);
							Gf += Delta;
							Gf = __USAT(Gf, 12);
							Rf -= Delta;
							if (Rf <= 0) Rf = 1;
						}
						*Dest++ = (Bf>>4);
						*Dest++ = (Gf>>4);
						*Dest++ = (Rf>>4);

						Bb = (((int32_t)(*(ImgBg)))*5 +
							(((int32_t)(*(ImgBg-LF))) + ((int32_t)(*(ImgBg+LF))))*4 +
							(((int32_t)(*(ImgBg-2))) + ((int32_t)(*(ImgBg+2))))/2 -
							((int32_t)(*(ImgBg-LF2))) - ((int32_t)(*(ImgBg+LF2))) -
							((int32_t)(*(ImgBg-LF-1))) - ((int32_t)(*(ImgBg-LF+1))) -
							((int32_t)(*(ImgBg+LF-1))) - ((int32_t)(*(ImgBg+LF+1)))) >> 3;
						if (Bb <= 0) Bb = 1;
						Bb = __USAT(Bb, 12);
						Gb = *(ImgBg);
						if (Gb == 0) Gb++;
						Rb = (((int32_t)(*(ImgBg)))*5 +
							(((int32_t)(*(ImgBg-1))) + ((int32_t)(*(ImgBg+1))))*4 +
							(((int32_t)(*(ImgBg-LF2))) + ((int32_t)(*(ImgBg+LF2))))/2 -
							((int32_t)(*(ImgBg-LF-1))) - ((int32_t)(*(ImgBg-LF+1))) -
							((int32_t)(*(ImgBg-2))) - ((int32_t)(*(ImgBg+2))) -
							((int32_t)(*(ImgBg+LF-1))) - ((int32_t)(*(ImgBg+LF+1)))) >> 3;
						if (Rb <= 0) Rb = 1;
						Rb = __USAT(Rb, 12);

						// Correction in case much BLUE
						if (Bb > Gb)
						{
							Delta = ((Bb-Gb)*Gb)/(148*16);
							Gb += Delta;
							Gb = __USAT(Gb, 12);
							Rb -= Delta;
							if (Rb <= 0) Rb = 1;
						}
					}
				}

				// Check a possible shadow (all colors decrease in shadow)
				if (((Rf-Rb) < (-5*16)) && ((Bf-Bb) < (-5*16)) && ((Gf-Gb) < (-5*16)))
				{
					Rratio = (Rf<<8)/Rb;
					Gratio = (Gf<<8)/Gb;
					Bratio = (Bf<<8)/Bb;
					if (((Rratio < (uint32_t)(0.4F*256)) || (Gratio < (uint32_t)(0.4F*256)) || (Bratio < (uint32_t)(0.4F*256))))
						Proc_SetMaskLinear(Index);
					else
					{
						// it can be shadow
						// max R, G, B
						if ((Rb >= Gb) && (Rb >= Bb))
						{
							Fract = (Rratio<<8)/Gratio;
							if ((Fract < (uint32_t)(0.8F*256)) || (Fract > (uint32_t)(1.2F*256)))
								Proc_SetMaskLinear(Index);
							else
							{
								Fract = (Rratio<<8)/Bratio;
								if ((Fract < (uint32_t)(0.8F*256)) || (Fract > (uint32_t)(1.2F*256)))
									Proc_SetMaskLinear(Index);
							}
						}
						else
						{
							if ((Gb >= Rb) && (Gb >= Bb))
							{
								Fract = (Gratio<<8)/Rratio;
								if ((Fract < (uint32_t)(0.8F*256)) || (Fract > (uint32_t)(1.2F*256)))
									Proc_SetMaskLinear(Index);
								else
								{
									Fract = (Gratio<<8)/Bratio;
									if ((Fract < (uint32_t)(0.8F*256)) || (Fract > (uint32_t)(1.2F*256)))
										Proc_SetMaskLinear(Index);
								}
							}
							else
							{
								// (Bb > Rb) && (Bb > Gb)
								Fract = (Bratio<<8)/Rratio;
								if ((Fract < (uint32_t)(0.8F*256)) || (Fract > (uint32_t)(1.2F*256)))
									Proc_SetMaskLinear(Index);
								else
								{
									Fract = (Bratio<<8)/Rratio;
									if ((Fract < (uint32_t)(0.8F*256)) || (Fract > (uint32_t)(1.2F*256)))
										Proc_SetMaskLinear(Index);
								}
							}
						}
					}
				}
				else
					// A significant increase of one color means an object
					if (((Rf-Rb) > ((3*Rb)/32)) || ((Bf-Bb) > ((3*Rb)/32)) || ((Gf-Gb) > ((3*Rb)/32)))
						Proc_SetMaskLinear(Index);
					else
						// check at least one color change
						if ((((Rf-Rb) < (-5*16)) || ((Bf-Bb) < (-5*16)) || ((Gf-Gb) < (-5*16))))
						{
							if (((Rb >= (250*16)) || (Gb >= (250*16)) || (Bb >= (250*16))))
							{
								LowTh  = (uint32_t)(0.5F*128);
								HighTh = (uint32_t)(1.5F*128);
							}
							else
							{
								LowTh  = (uint32_t)(0.8F*128);
								HighTh = (uint32_t)(1.2F*128);
							}
							if ((Rb >= Gb) && (Rb >= Bb))
							{
								Fract  = (Gb*Rf<<7)/(Gf*Rb);
								if ((Fract < LowTh) || (Fract > HighTh))
									Proc_SetMaskLinear(Index);
								else
								{
									Fract  = (Bb*Rf<<7)/(Bf*Rb);
									if ((Fract < LowTh) || (Fract > HighTh))
										Proc_SetMaskLinear(Index);
								}
							}
							else
							{
								if ((Gb >= Rb) && (Gb >= Bb))
								{
									Fract  = (Rb*Gf<<7)/(Rf*Gb);
									if ((Fract < LowTh) || (Fract > HighTh))
										Proc_SetMaskLinear(Index);
									else
									{
										Fract  = (Bb*Gf<<7)/(Bf*Gb);
										if ((Fract < LowTh) || (Fract > HighTh))
											Proc_SetMaskLinear(Index);
									}
								}
								else
								{
									//(Bb > Rb) && (Bb > Gb)
									Fract  = (Rb*Bf<<7)/(Rf*Bb);
									if ((Fract < LowTh) || (Fract > HighTh))
										Proc_SetMaskLinear(Index);
									else
									{
										Fract  = (Gb*Bf<<7)/(Gf*Bb);
										if ((Fract < LowTh) || (Fract > HighTh))
											Proc_SetMaskLinear(Index);
									}
								}
							}
						}
						// else same object

				Index++;
				ImgFg++;
				ImgBg++;
			}
			Utils_MdmaClearFlags(MDMA_Channel1);
			Utils_MdmaTransfer(MDMA_Channel1, (uint32_t)Proc_BufferRgbLine, DestRGB, L*3);
			DestRGB += L*3;
			ImgFg += 4;
			ImgBg += 4;
			Proc_CheckRainDropReflection(K*28 + J);
			Utils_MdmaPoolTransferComplete(MDMA_Channel1);
		}

		if (K == 34) break;
		if (K == 33) Z = 8;

		SrcDmaFg +=  28*LF2;
		SrcDmaBg +=  28*LF2;
		Utils_MdmaClearFlags(MDMA_Channel1);
		Utils_MdmaTransfer(MDMA_Channel0, SrcDmaFg, (uint32_t)Proc_BufferMdmaOne, (Z+4)*LF2);
		Utils_MdmaTransfer(MDMA_Channel1, SrcDmaBg, (uint32_t)Proc_BufferMdmaTwo, (Z+4)*LF2);

		ImgFg = Proc_BufferMdmaOne + LF2 + 2;
		ImgBg = Proc_BufferMdmaTwo + LF2 + 2;
	}

	if (Proc_ResultsRainDropReflection())
	{
		DevStatus[DEVSTATUS_RAIN_DETECTED_STATUS_IDX]	= DEVSTATUS_RAIN_DETECTED_YES;
		return -10;
	}


	Proc_ImgMask    = Proc_ImgMaskOne;
	Proc_ImgMaskSec = Proc_ImgMaskTwo;

	// Proceed to open the image
	// Remove noise (erosion square 3x3)
	Proc_MaskErosionSquare3x3();
	// Return to current shape (dilation square 3x3)
	Proc_MaskDilationSquare3x3();
	// Clear margins
	Proc_ClearMaskMargins(Proc_ImgMask);

	///////////////////////////////////////////////////////////
	// Find the boundary box (Xmin, Ymin, Xmax, Ymax)
	///////////////////////////////////////////////////////////
	int32_t Xmin = 0x7FFFFFFF;
	int32_t Ymin = 0x7FFFFFFF;
	int32_t Xmax = 0x0;
	int32_t Ymax = 0x0;
    for (Y=0; Y<L; Y++)
    {
    	for (X=0; X<L; X++)
       		if (Proc_GetMaskXY(X, Y))
       		{
				if (Xmin > X)
					Xmin = X;
				if (Ymin > Y)
					Ymin = Y;
				if (Xmax < X)
					Xmax = X;
				if (Ymax < Y)
					Ymax = Y;
       		}
    }

	if (Xmin == 0x7FFFFFFF)
		return -1;

	if (--Xmin <= 0)	Proc_XMin = 0;
	else				Proc_XMin = Xmin & 0xFFF8;
	if (--Ymin <= 0)	Proc_YMin = 0;
	else				Proc_YMin = Ymin & 0xFFF8;
	Xmax += 9;
	if (Xmax >= L)		Proc_XMax = L;
	else				Proc_XMax = Xmax & 0xFFF8;
	Ymax += 9;
	if (Ymax >= L)		Proc_YMax = L;
	else				Proc_YMax = Ymax & 0xFFF8;

	Proc_SizeX = Proc_XMax - Proc_XMin;
	Proc_SizeY = Proc_YMax - Proc_YMin;

	// Fill small gaps and closed islands
    Proc_MaskFillSmallGaps();
    // Make a copy of mask in secondary mask (for future use)
    Proc_CopyMaskAll(Proc_ImgMask, Proc_ImgMaskSec);
	return 1;
}


void __attribute__((section(".fast_code_sec"))) Proc_AdjustImageToOrigin(void)
{
	// Adjust BGR image
	uint32_t Length		= Proc_SizeX*3;
	uint32_t DestDma	= (uint32_t)Utils_GetDemosaicImgPtr();
	uint32_t SrcDma 	= DestDma + (Proc_YMin*L + Proc_XMin)*3;
	uint32_t SizeY		= Proc_SizeY;

	Utils_MdmaClearFlags(MDMA_Channel1);
	Utils_MdmaTransfer(MDMA_Channel1, SrcDma, DestDma, Length);
	SizeY--;

	while (SizeY--)
	{
		Utils_MdmaPoolTransferComplete(MDMA_Channel1);
		SrcDma += L*3;
		Utils_MdmaClearFlags(MDMA_Channel1);
		Utils_MdmaTransferKeepDest(MDMA_Channel1, SrcDma, Length);
	}
	Utils_MdmaPoolTransferComplete(MDMA_Channel1);

	// Clear 32 more rows for contours data
	*Proc_BufferCopyRgb = NC_BACKGROUND_CONTOUR_TEXT_COLOR | (NC_BACKGROUND_CONTOUR_TEXT_COLOR<<8);
	SrcDma = (uint32_t)Proc_BufferCopyRgb;
	DestDma += Proc_SizeX*Proc_SizeY*3;
	Length = Proc_SizeX*32*3;
	Utils_MdmaClearFlags(MDMA_Channel2);
	if (Length < 0xFFFF)
		Utils_MdmaTransfer(MDMA_Channel2, SrcDma, DestDma, Length);
	else
	{
		Utils_MdmaTransfer(MDMA_Channel2, SrcDma, DestDma, Length/2);
		Utils_MdmaPoolTransferComplete(MDMA_Channel2);
		Utils_MdmaClearFlags(MDMA_Channel2);
		Utils_MdmaTransferKeepSrcDest(MDMA_Channel2, Length/2);
	}
	Utils_MdmaPoolTransferComplete(MDMA_Channel2);
}


uint32_t __attribute__((section(".fast_code_sec"))) Proc_ParseContours(void)
{
	int32_t	 I;
	uint32_t ContourXMin;
	uint32_t ContourXMax;
	uint32_t ContourYMin;
	uint32_t ContourYMax;
	uint32_t CurrentContourX;
	uint32_t CurrentContourY;
	uint32_t TmpContourX;
	uint32_t TmpContourY;
	uint32_t Direction;
	uint32_t CurrentContourNumber  = 0;
	uint8_t* CurrentContourDirection = Proc_ContourDirections;
	uint32_t Points;
	uint32_t X = Proc_XMin;
	uint32_t Y = Proc_YMin;
	uint32_t Index = Y*L + X;

	do
	{
		Proc_Contour[CurrentContourNumber].Directions = CurrentContourDirection;
		for (; Y < Proc_YMax; Y++)
		{
			for (; X < Proc_XMax; X++)
			{
				if (Proc_GetMaskLinear(Index))
				{
					if (Proc_GetMaskLinear(Index-1) ||
						Proc_GetMaskLinear(Index+1) ||
						Proc_GetMaskLinear(Index-L-1) ||
						Proc_GetMaskLinear(Index-L) ||
						Proc_GetMaskLinear(Index-L+1) ||
						Proc_GetMaskLinear(Index+L-1) ||
						Proc_GetMaskLinear(Index+L) ||
						Proc_GetMaskLinear(Index+L+1))
						break;
					else
						Proc_ImgMask[Index>>3] &= ~(1 << (Index & 0x07));
				}
				Index++;
			}
			if (X < Proc_XMax) break;
			X = Proc_XMin;
			Index += L - Proc_SizeX;
		}

		if (Y == Proc_YMax) break;

		for (I=Y; I<L; I++)
		{
			Proc_ContourXMinMax[I][0] = 0xFFFFFFFF;
			Proc_ContourXMinMax[I][1] = 0x00000000;
		}

		CurrentContourX = X;
		ContourXMin		= X;
		ContourXMax		= X;
		CurrentContourY = Y;
		ContourYMin		= Y;
		ContourYMax		= Y;
		Direction 		= 7;

		do
		{
			Direction += 6 - (Direction & 0x01);
			do {
				Direction = (Direction + 1) % 8;
				TmpContourX = CurrentContourX + Proc_DirectionAddX[Direction];
				TmpContourY = CurrentContourY + Proc_DirectionAddY[Direction];
			} while ((Proc_ImgMask[(TmpContourY*L + TmpContourX)>>3] & (1 << (TmpContourX & 0x07))) == 0);

			*CurrentContourDirection++ = Direction;
			CurrentContourX = TmpContourX;
			CurrentContourY = TmpContourY;

			if (CurrentContourX < Proc_ContourXMinMax[TmpContourY][0])
				Proc_ContourXMinMax[TmpContourY][0] = CurrentContourX;
			if (CurrentContourX > Proc_ContourXMinMax[TmpContourY][1])
				Proc_ContourXMinMax[TmpContourY][1] = CurrentContourX;
			if (CurrentContourY > ContourYMax)
				ContourYMax = CurrentContourY;
			if (CurrentContourX > ContourXMax)
				ContourXMax = CurrentContourX;
			if (CurrentContourX < ContourXMin)
				ContourXMin = CurrentContourX;
		} while ((CurrentContourX != X) || (CurrentContourY != Y));

		// Clear contour from the mask
		uint32_t IndexXmin, IndexXmax;
		for (I = ContourYMin; I <= ContourYMax; I++)
		{
			IndexXmin = I*L + Proc_ContourXMinMax[I][0];
			IndexXmax = I*L + Proc_ContourXMinMax[I][1];
			while (IndexXmin <= IndexXmax)
			{
				Proc_ImgMask[IndexXmin>>3] &= ~(1 << (IndexXmin & 0x07));
				IndexXmin++;
			}
		}

		Points = CurrentContourDirection - Proc_Contour[CurrentContourNumber].Directions;
		// Filter out small contours
		if (Points < NC_CONTOUR_MIN_PERIMETER)
			CurrentContourDirection = Proc_Contour[CurrentContourNumber].Directions;
		else
		{
			Proc_Contour[CurrentContourNumber].PointsNumber = Points;
			Proc_Contour[CurrentContourNumber].XStart 	= X;
			Proc_Contour[CurrentContourNumber].YStart 	= Y;
			Proc_Contour[CurrentContourNumber].XMin		= ContourXMin;
			Proc_Contour[CurrentContourNumber].YMin	 	= ContourYMin;
			Proc_Contour[CurrentContourNumber].XSize 	= ContourXMax - ContourXMin + 1;
			Proc_Contour[CurrentContourNumber].YSize 	= ContourYMax - ContourYMin + 1;
			CurrentContourNumber++;
		}
	} while (CurrentContourNumber < NC_MAX_VALID_CONTOURS);

	//Store the number of contours detected
	Proc_LastContourDirection = CurrentContourDirection;
	Proc_NumberOfContours = CurrentContourNumber;
	return CurrentContourNumber;
}


void __attribute__((section(".fast_code_sec"))) Proc_RemoveCloseContours(void)
{
	uint32_t I, J;
	struct Proc_ContourStruct *ContourBig;
	struct Proc_ContourStruct *ContourSmall;
	struct Proc_ContourStruct Contour;
	int32_t  XCenterBig, YCenterBig;
	int32_t  XMinBig, YMinBig, XMaxBig, YMaxBig;
	uint32_t ContourBigPointsNumber;
	uint32_t NumberOfContours = Proc_NumberOfContours;


	if (NumberOfContours < 2) return;
	for (I=0; I<(NumberOfContours-1); I++)
	{
		for (J=I+1; J<NumberOfContours; J++)
		{
			if ((Proc_Contour[I].PointsNumber <= NC_CONTOUR_RESIDUAL_MAX_PERIMETER) || (Proc_Contour[J].PointsNumber <= NC_CONTOUR_RESIDUAL_MAX_PERIMETER))
			{
				if (Proc_Contour[I].PointsNumber > Proc_Contour[J].PointsNumber)
				{
					ContourBig   = &Proc_Contour[I];
					ContourSmall = &Proc_Contour[J];
				}
				else
				{
					ContourBig   = &Proc_Contour[J];
					ContourSmall = &Proc_Contour[I];
				}
				ContourBigPointsNumber = (ContourBig->PointsNumber)/4;
				XCenterBig = ContourBig->XMin + ContourBig->XSize/2;
				YCenterBig = ContourBig->YMin + ContourBig->YSize/2;
				XMinBig = XCenterBig - ContourBigPointsNumber;
				XMaxBig = XCenterBig + ContourBigPointsNumber;
				YMinBig = YCenterBig - ContourBigPointsNumber;
				YMaxBig = YCenterBig + ContourBigPointsNumber;

				if ((XMinBig <= ContourSmall->XMin) &&
					(YMinBig <= ContourSmall->YMin) &&
					(XMaxBig >= (ContourSmall->XMin + ContourSmall->XSize)) &&
					(YMaxBig >= (ContourSmall->YMin + ContourSmall->YSize)))
				{
					//Contour shall be removed
					*ContourSmall = Proc_Contour[--NumberOfContours];
					if (ContourBig == &Proc_Contour[I])
						J--;
					else
					{
						I--;
						break;
					}
				}
			}
		}
	}
	Proc_NumberOfRemovedContours = Proc_NumberOfContours - NumberOfContours;
	Proc_NumberOfContours		 = NumberOfContours;

	//Bubble sort contours on size (descending)
	if (NumberOfContours < 2) return;
	uint32_t Swaps = 1;
	for (I=NumberOfContours; ((I>0)&&(Swaps)); I--)
	{
		Swaps = 0;
		for (J=0; J<I-1; J++)
			if (Proc_Contour[J].PointsNumber < Proc_Contour[J+1].PointsNumber)
			{
				Contour = Proc_Contour[J];
				Proc_Contour[J] = Proc_Contour[J+1];
				Proc_Contour[J+1] = Contour;
				Swaps = 1;
			}
	}
}


void __attribute__((section(".fast_code_sec"))) Proc_AdjustImageToValidContours(void)
{
	uint32_t I;
	struct Proc_ContourStruct *Contour;
	int32_t  XCenter, YCenter;
	int32_t  XMin, YMin, XMax, YMax;
	uint32_t ContourPointsNumber;
	int32_t XMinNew = L;
	int32_t YMinNew = L;
	int32_t XMaxNew = 0;
	int32_t YMaxNew = 0;
	int32_t Diff;
	int32_t Max;

	for (I=0; (I<Proc_NumberOfContours) && (I<3); I++)
	{
		Contour = &Proc_Contour[I];
		ContourPointsNumber = (Contour->PointsNumber)/4;
		XCenter = Contour->XMin + Contour->XSize/2;
		YCenter = Contour->YMin + Contour->YSize/2;
		XMin = XCenter - ContourPointsNumber;
		XMax = XCenter + ContourPointsNumber;
		YMin = YCenter - ContourPointsNumber;
		YMax = YCenter + ContourPointsNumber;

		if ((Contour->XMin) < XMin)
			XMin = Contour->XMin;
		else
		{
			Diff = XMin - (Contour->XMin);
			if (Diff < -32)
				XMin = (Contour->XMin) - 32;
		}
		if (XMin < XMinNew)
			XMinNew = XMin;

		if ((Contour->YMin) < YMin)
			YMin = Contour->YMin;
		else
		{
			Diff = YMin - (Contour->YMin);
			if (Diff < -32)
				YMin = (Contour->YMin) - 32;
		}
		if (YMin < YMinNew)
			YMinNew = YMin;

		Max = Contour->XMin + Contour->XSize;
		if (Max > XMax)
			XMax = Max;
		else
		{
			Diff = XMax - Max;
			if (Diff > 32)
				XMax = Max + 32;
		}
		if (XMax > XMaxNew)
			XMaxNew = XMax;

		Max = Contour->YMin + Contour->YSize;
		if (Max > YMax)
			YMax = Max;
		else
		{
			Diff = YMax - Max;
			if (Diff > 32)
				YMax = Max + 32;
		}
		if (YMax > YMaxNew)
			YMaxNew = YMax;
	}

	Proc_NumberOfRemovedContours += Proc_NumberOfContours - I;
	Proc_NumberOfContours = I;

	if (--XMinNew <= 0)	XMinNew = 0;
	else				XMinNew = XMinNew & 0xFFF8;
	if (--YMinNew <= 0)	YMinNew = 0;
	else				YMinNew = YMinNew & 0xFFF8;
	XMaxNew += 9;
	if (XMaxNew >= L)	XMaxNew = L;
	else				XMaxNew = XMaxNew & 0xFFF8;
	YMaxNew += 9;
	if (YMaxNew >= L)	YMaxNew = L;
	else				YMaxNew = YMaxNew & 0xFFF8;

	uint32_t SizeXNew = XMaxNew - XMinNew;
	uint32_t 				   		SizeNewMin = (12)*8;
	if (Proc_NumberOfContours == 2) SizeNewMin = (12+1+12)*8;
	if (Proc_NumberOfContours == 3) SizeNewMin = (12+1+12+1+12)*8;
	if (SizeXNew < SizeNewMin)
	{
		uint32_t Diff  = (SizeNewMin - SizeXNew)>>3;
		uint32_t Diff2 = Diff>>1;
		XMinNew -= Diff2<<3;
		XMaxNew += (Diff2<<3) + ((Diff & 0x01)<<3);
		if (XMinNew < 0)
		{
			XMaxNew -= XMinNew;
			XMinNew = 0;
		}
		if (XMaxNew > L)
		{
			XMinNew -= (XMaxNew - L);
			XMaxNew = L;
		}
	}

	uint32_t SizeYNew = YMaxNew - YMinNew;
	if (SizeYNew < 32)
	{
		SizeYNew = 32 - SizeYNew;
		YMinNew -= SizeYNew/2;
		YMaxNew += SizeYNew/2;
		if (YMinNew <= 0)
		{
			YMaxNew -= YMinNew;
			YMinNew = 0;
		}
		if (YMaxNew >= L)
		{
			YMinNew -= YMaxNew - L;
			YMaxNew = L;
		}
	}

	Proc_XMin = XMinNew;
	Proc_YMin = YMinNew;
	Proc_SizeX = XMaxNew - XMinNew;
	Proc_SizeY = YMaxNew - YMinNew;
}



void __attribute__((section(".fast_code_sec"))) Proc_FastVectorMedianFilter(void)
{
	uint32_t LS = Proc_SizeX*3;
	uint32_t RestLines = Proc_SizeY;
	uint32_t LinesToCopyOnce;
	uint8_t* Src;
	uint32_t MaxLinesInBuffer;
	uint32_t SrcImg = (uint32_t)Utils_GetDemosaicImgPtr();

	int32_t I, J;
	int32_t R0, G0, B0;
	int32_t R1, G1, B1;
	int32_t R2, G2, B2;
	int32_t R3, G3, B3;
	int32_t R4, G4, B4;
	int32_t Delta12, Delta23, Delta34, Delta41, Delta13, Delta24;
	int32_t Sum0, SumW, SumMin;

	Utils_MdmaClearFlags(MDMA_Channel0);
	Utils_MdmaTransfer(MDMA_Channel0, SrcImg, (uint32_t)Proc_BufferMdmaOne, LS*2);
	SrcImg 	  		+= LS*2;
	RestLines 		-= 2;
	MaxLinesInBuffer = ((64*1024) / LS) - 2;
	Utils_MdmaPoolTransferComplete(MDMA_Channel0);

	while (1)
	{
		LinesToCopyOnce = MaxLinesInBuffer;
		if (LinesToCopyOnce > RestLines)
			LinesToCopyOnce = RestLines;
		RestLines -= LinesToCopyOnce;

		Utils_MdmaClearFlags(MDMA_Channel0);
		Utils_MdmaTransfer(MDMA_Channel0, SrcImg, (uint32_t)Proc_BufferMdmaOne + LS*2, LinesToCopyOnce*LS);
		Src = ((uint8_t*)Proc_BufferMdmaOne) + LS + 3;
		J	= LinesToCopyOnce;
		Utils_MdmaPoolTransferComplete(MDMA_Channel0);

		while (J--)
		{
			I = Proc_SizeX - 2;
			while(I--)
			{
				B0 = Src[0];
				G0 = Src[1];
				R0 = Src[2];

				B1 = *(Src-3);
				G1 = *(Src-2);
				R1 = *(Src-1);
				B3 = *(Src+3);
				G3 = *(Src+4);
				R3 = *(Src+5);
				B2 = *(Src-LS);
				G2 = *(Src-LS+1);
				R2 = *(Src-LS+2);
				B4 = *(Src+LS);
				G4 = *(Src+LS+1);
				R4 = *(Src+LS+2);

				Sum0 = (B0-B1)*(B0-B1) + (G0-G1)*(G0-G1) + (R0-R1)*(R0-R1) +
					   (B0-B2)*(B0-B2) + (G0-G2)*(G0-G2) + (R0-R2)*(R0-R2) +
					   (B0-B3)*(B0-B3) + (G0-G3)*(G0-G3) + (R0-R3)*(R0-R3) +
					   (B0-B4)*(B0-B4) + (G0-G4)*(G0-G4) + (R0-R4)*(R0-R4);

				Delta12 = (B1-B2)*(B1-B2) + (G1-G2)*(G1-G2) + (R1-R2)*(R1-R2);
				Delta23 = (B2-B3)*(B2-B3) + (G2-G3)*(G2-G3) + (R2-R3)*(R2-R3);
				Delta34 = (B3-B4)*(B3-B4) + (G3-G4)*(G3-G4) + (R3-R4)*(R3-R4);
				Delta41 = (B4-B1)*(B4-B1) + (G4-G1)*(G4-G1) + (R4-R1)*(R4-R1);
				Delta13 = (B1-B3)*(B1-B3) + (G1-G3)*(G1-G3) + (R1-R3)*(R1-R3);
				Delta24 = (B2-B4)*(B2-B4) + (G2-G4)*(G2-G4) + (R2-R4)*(R2-R4);

				SumMin = Sum0;
				SumW = Delta12 + Delta13 + Delta41;
				if (SumMin > SumW)
				{
					SumMin = SumW;
					B0 = B1;
					G0 = G1;
					R0 = R1;
				}
				SumW = Delta12 + Delta23 + Delta24;
				if (SumMin > SumW)
				{
					SumMin = SumW;
					B0 = B2;
					G0 = G2;
					R0 = R2;
				}
				SumW = Delta13 + Delta23 + Delta34;
				if (SumMin > SumW)
				{
					SumMin = SumW;
					B0 = B3;
					G0 = G3;
					R0 = R3;
				}
				SumW = Delta41 + Delta24 + Delta34;
				if (SumMin > SumW)
				{
					SumMin = SumW;
					B0 = B4;
					G0 = G4;
					R0 = R4;
				}

				if ((Sum0 - SumMin) > (3*65536/2))
				{
					Src[0] = B0;
					Src[1] = G0;
					Src[2] = R0;
				}

				Src += 3;
			}
			Src += 3*2;
		}

		// Store RGB buffer in memory
		Utils_MdmaClearFlags(MDMA_Channel0);
		Utils_MdmaTransfer(MDMA_Channel0, (uint32_t)Proc_BufferMdmaOne + LS, SrcImg-LS, LinesToCopyOnce*LS);
		SrcImg += LinesToCopyOnce*LS;
		Utils_MdmaPoolTransferComplete(MDMA_Channel0);

		if (RestLines == 0) break;
		// Copy last 2 lines at the beginning
		I = LS/2;
		uint32_t* SrcTmp  = (uint32_t*)(Src-3-LS);
		uint32_t* DestTmp = (uint32_t*)Proc_BufferMdmaOne;
		while (I--)
			*(DestTmp++) = *(SrcTmp++);
	}
}


void __attribute__((section(".fast_code_sec"))) Proc_ApplyBrightness(int32_t BrightnessPercent)
{
	uint32_t LS = Proc_SizeX*3;
	uint32_t RestLines = Proc_SizeY;
	uint32_t LinesToCopyOnce;
	uint8_t* Src;
	uint32_t MaxLinesInBuffer = ((64*1024) / LS) - 2;
	uint32_t SrcImg = (uint32_t)Utils_GetDemosaicImgPtr();
	uint32_t Color;
	int32_t I;

	BrightnessPercent = 256 + (256*BrightnessPercent)/100;

	do
	{
		LinesToCopyOnce = MaxLinesInBuffer;
		if (LinesToCopyOnce > RestLines)
			LinesToCopyOnce = RestLines;
		RestLines -= LinesToCopyOnce;

		Utils_MdmaClearFlags(MDMA_Channel0);
		Utils_MdmaTransfer(MDMA_Channel0, SrcImg, (uint32_t)Proc_BufferMdmaOne, LinesToCopyOnce*LS);
		Src = ((uint8_t*)Proc_BufferMdmaOne);
		Utils_MdmaPoolTransferComplete(MDMA_Channel0);

		I = LinesToCopyOnce*Proc_SizeX*3;
		while (I--)
		{
			Color = (BrightnessPercent * (*Src)) >> 8;
			Color = __USAT(Color, 8);
			*Src++ = Color;
		}

		// Store RGB buffer in memory
		Utils_MdmaClearFlags(MDMA_Channel0);
		Utils_MdmaTransfer(MDMA_Channel0, (uint32_t)Proc_BufferMdmaOne, SrcImg, LinesToCopyOnce*LS);
		SrcImg += LinesToCopyOnce*LS;
		Utils_MdmaPoolTransferComplete(MDMA_Channel0);
	} while (RestLines != 0);
}


void __attribute__((section(".fast_code_sec"))) Proc_DrawContours(void)
{
	struct Proc_ContourStruct* Contour;
	uint8_t* DestThumb;
	uint8_t* Dest;
	uint8_t* DestFirst;
	uint8_t* DirectionsPtr;
	uint32_t Direction;
	//uint32_t ContourColor;
	//uint8_t  R, G, B;
	int32_t  DirectionsAdd[8];
	uint32_t X, Y;
	uint32_t ScaleUp, ScaleDown;

	DirectionsAdd[0] = 3;
	DirectionsAdd[4] = -DirectionsAdd[0];
	DirectionsAdd[1] = (Proc_SizeX-1)*(-3);
	DirectionsAdd[5] = -DirectionsAdd[1];
	DirectionsAdd[2] = (Proc_SizeX)*(-3);
	DirectionsAdd[6] = -DirectionsAdd[2];
	DirectionsAdd[3] = (Proc_SizeX+1)*(-3);
	DirectionsAdd[7] = -DirectionsAdd[3];

	// Clear contours thumb nails
	uint32_t *Dest32 = (uint32_t*)Proc_ContoursThumbnails;
	while (Dest32 != (uint32_t*)(Proc_ContoursThumbnails+3*32*32))
		*Dest32++ = 0x00000000;

	for (uint32_t C=0; C<Proc_NumberOfContours; C++)
	{
		Contour = &Proc_Contour[C];
		X = Contour->XStart;
		Y = Contour->YStart;
		Dest = (uint8_t*)Utils_GetDemosaicImgPtr() + ((Contour->YStart - Proc_YMin)*Proc_SizeX + (Contour->XStart - Proc_XMin))*3;
		DestFirst = Dest;
		DirectionsPtr = Contour->Directions;

		ScaleDown = Contour->XSize;
		if (ScaleDown < (Contour->YSize))
			ScaleDown = Contour->YSize;
		if (ScaleDown <= 30) {ScaleUp = 1;  ScaleDown = 1;}
		else		   		 {ScaleUp = 30;}
		DestThumb = Proc_ContoursThumbnails + C*32*32 + (30 - (Contour->XSize*ScaleUp)/ScaleDown) + 32*(((32 - (Contour->YSize*ScaleUp)/ScaleDown)/2));

		//ContourColor			 = NC_CONTOUR1_RGB_COLOR;
		//if (C == 1) ContourColor = NC_CONTOUR2_RGB_COLOR;
		//if (C == 2) ContourColor = NC_CONTOUR3_RGB_COLOR;
		//R = (ContourColor)>>16;
		//G = (ContourColor<<16)>>24;
		//B = (ContourColor);

		X -= Contour->XMin;
		Y -= Contour->YMin;
		do
		{
			// Set big contour
			//Dest[0] = B;
			//Dest[1] = G;
			//Dest[2] = R;
			// Set thumb nail mask
			DestThumb[(((Y*ScaleUp)/ScaleDown)<<5) + ((X*ScaleUp)/ScaleDown)] = 1;

			Direction = *DirectionsPtr++;
			X 	 += Proc_DirectionAddX[Direction];
			Y 	 += Proc_DirectionAddY[Direction];
			Dest += DirectionsAdd[Direction];
		} while (Dest != DestFirst);
	}
}


void __attribute__((section(".fast_code_sec"))) Proc_AddContoursDataToImage(void)
{
	struct Proc_ContourStruct* Contour;
	char Text[16];
	uint32_t ContourColor;
	uint8_t* SrcThumbMask;
	uint8_t  R, G, B;
	uint32_t X, Y;
	uint32_t PosX, AddNext;

	AddNext = (((Proc_SizeX>>3) - Proc_NumberOfContours*12)/(Proc_NumberOfContours+1))<<3;
	PosX	= AddNext;
	if (AddNext == 0)	AddNext = 8;

	for (uint32_t C=0; C<Proc_NumberOfContours; C++)
	{
		Contour = &Proc_Contour[C];
		ContourColor			 = NC_CONTOUR1_RGB_COLOR;
		if (C == 1)	ContourColor = NC_CONTOUR2_RGB_COLOR;
		if (C == 2)	ContourColor = NC_CONTOUR3_RGB_COLOR;
		R = (ContourColor)>>16;
		G = (ContourColor<<16)>>24;
		B = (ContourColor);

		Font_ClearLocalDisplay(NC_BACKGROUND_CONTOUR_TEXT_COLOR);
		Font_SetCursorXY(0, 0);
		Font_WriteText(Contour->ExtraData->ShortName, NC_CONTOUR_TEXT_COLOR);
		Font_SetCursorXY(0, 8);
		strcpy(Text, "L=");
		strcat(Text, Utils_Ftoa1(Contour->ExtraData->Length));
		strcat(Text, "mm");
		Font_WriteText(Text, NC_CONTOUR_TEXT_COLOR);
		Font_SetCursorXY(0, 16);
		strcpy(Text, "W=");
		strcat(Text, Utils_Ftoa1(Contour->ExtraData->Width));
		strcat(Text, "mm");
		Font_WriteText(Text, NC_CONTOUR_TEXT_COLOR);
		Font_SetCursorXY(0, 24);
		strcpy(Text, "\x80=");
		strcat(Text, Utils_Ftoa0(Contour->ExtraData->AngleDeg));
		strcat(Text, "\x81");
		Font_WriteText(Text, NC_CONTOUR_TEXT_COLOR);
		Font_WriteText("\x82", Contour->ExtraData->ColorRgbAvg);

		uint8_t* DestRgb = (uint8_t*)Utils_GetDemosaicImgPtr() + (Proc_SizeX*Proc_SizeY + PosX)*3;
		Font_CopyDisplay(8*8, 4*8, DestRgb + 32*3, Proc_SizeX);

		// Add thumbnail to picture
		SrcThumbMask = Proc_ContoursThumbnails + C*32*32;
		Y=32;
		while (Y--)
		{
			X=32;
			while (X--)
				if (*SrcThumbMask++) {*DestRgb++ = B; *DestRgb++ = G; *DestRgb++ = R;}
				else DestRgb += 3;
			DestRgb += (Proc_SizeX - 32)*3;
		}
		PosX += AddNext + 12*8;
	}
}


void __attribute__((section(".fast_code_sec"))) Proc_StorePointX(uint32_t X, uint32_t Y)
{
	uint32_t PrevContourX = 0xFFFFFFFF;
	uint32_t ContourX =		Proc_ContourSegmentsY[Y];

	Proc_ContourSegmentsX[Proc_ContourFreeX].X = X;
	while ((ContourX>>15) == 0)
	{
		if (X < Proc_ContourSegmentsX[ContourX].X)
			break;
		PrevContourX = ContourX;
		ContourX = Proc_ContourSegmentsX[ContourX].NextX;
	}
	Proc_ContourSegmentsX[Proc_ContourFreeX].NextX = ContourX;
	if (PrevContourX == 0xFFFFFFFF)
		Proc_ContourSegmentsY[Y] = Proc_ContourFreeX;
	else
		Proc_ContourSegmentsX[PrevContourX].NextX = Proc_ContourFreeX;
	Proc_ContourFreeX++;
}


uint32_t __attribute__((section(".fast_code_sec"))) Proc_DirectionType(uint32_t Direction)
{
	if (Direction == 0)	return PROC_DIR_TYPE_HPLUS;
	if (Direction == 4)	return PROC_DIR_TYPE_HMINUS;
	if (Direction > 4)	return PROC_DIR_TYPE_BEGIN;
	return PROC_DIR_TYPE_END;
}


uint32_t __attribute__((section(".fast_code_sec"))) Proc_UpdateContourAfterRemovals(struct Proc_ContourStruct* Contour)
{
	uint32_t I;
	uint32_t Direction;
	uint8_t* CurrentContourDirection;
	uint32_t Points;
	uint32_t TmpContourX, TmpContourY;
	uint32_t CurrentContourX, CurrentContourY;
	uint32_t ContourXMin, ContourXMax, ContourYMin, ContourYMax;
	uint32_t IndexXmin, IndexXmax;
	uint32_t X = Contour->XMin;
	uint32_t XMin = X;
	uint32_t XMax = X + (Contour->XSize);
	uint32_t Y = Contour->YMin;
	uint32_t YMax = Y + (Contour->YSize);

	uint32_t Index = Y*L + X;
	uint32_t ContourFound = 0;

	while(1)
	{
		CurrentContourDirection = Proc_LastContourDirection;
		for (; Y < YMax; Y++)
		{
			for (; X < XMax; X++)
			{
				if (Proc_GetMaskLinear(Index))
				{
					if (Proc_GetMaskLinear(Index-1) ||
						Proc_GetMaskLinear(Index+1) ||
						Proc_GetMaskLinear(Index-L-1) ||
						Proc_GetMaskLinear(Index-L) ||
						Proc_GetMaskLinear(Index-L+1) ||
						Proc_GetMaskLinear(Index+L-1) ||
						Proc_GetMaskLinear(Index+L) ||
						Proc_GetMaskLinear(Index+L+1))
						break;
					else
						Proc_ImgMask[Index>>3] &= ~(1 << (Index & 0x07));
				}
				Index++;
			}
			if (X < XMax) break;
			X = XMin;
			Index += L + XMin - XMax;
		}

		if (Y == YMax) break;

		for (I=Y; I<YMax; I++)
		{
			Proc_ContourXMinMax[I][0] = 0xFFFFFFFF;
			Proc_ContourXMinMax[I][1] = 0x00000000;
		}

		CurrentContourX = X;
		ContourXMin		= X;
		ContourXMax		= X;
		CurrentContourY = Y;
		ContourYMin		= Y;
		ContourYMax		= Y;
		Direction		= 7;

		do
		{
			Direction += 6 - (Direction & 0x01);
			do {
				Direction = (Direction + 1) % 8;
				TmpContourX = CurrentContourX + Proc_DirectionAddX[Direction];
				TmpContourY = CurrentContourY + Proc_DirectionAddY[Direction];
			} while ((Proc_ImgMask[(TmpContourY*L + TmpContourX)>>3] & (1 << (TmpContourX & 0x07))) == 0);

			if (ContourFound == 0)
				*CurrentContourDirection++ = Direction;
			CurrentContourX = TmpContourX;
			CurrentContourY = TmpContourY;

			if (CurrentContourX < Proc_ContourXMinMax[TmpContourY][0])
				Proc_ContourXMinMax[TmpContourY][0] = CurrentContourX;
			if (CurrentContourX > Proc_ContourXMinMax[TmpContourY][1])
				Proc_ContourXMinMax[TmpContourY][1] = CurrentContourX;
			if (CurrentContourY > ContourYMax)
				ContourYMax = CurrentContourY;
			if (CurrentContourX > ContourXMax)
				ContourXMax = CurrentContourX;
			if (CurrentContourX < ContourXMin)
				ContourXMin = CurrentContourX;
		} while ((CurrentContourX != X) || (CurrentContourY != Y));

		Points = CurrentContourDirection - Proc_LastContourDirection;
		// Take as valid one contour at least half of the previous one
		if ((ContourFound == 0) && (Points > (Contour->PointsNumber/2)))
		{
			Contour->Directions			= Proc_LastContourDirection;
			Proc_LastContourDirection	= CurrentContourDirection;
			Contour->PointsNumber 		= Points;
			Contour->XMin  		  		= ContourXMin;
			Contour->YMin  				= ContourYMin;
			Contour->XSize 				= ContourXMax - ContourXMin + 1;
			Contour->YSize 				= ContourYMax - ContourYMin + 1;
			Contour->XStart 			= X;
			Contour->YStart 			= Y;
			ContourFound 				= 1;
		}
		else
			// Clear contour from the mask
			for (I = ContourYMin; I <= ContourYMax; I++)
			{
				IndexXmin = I*L + Proc_ContourXMinMax[I][0];
				IndexXmax = I*L + Proc_ContourXMinMax[I][1];
				while (IndexXmin <= IndexXmax)
				{
					Proc_ImgMask[IndexXmin>>3] &= ~(1 << (IndexXmin & 0x07));
					IndexXmin++;
				}
			}
	}
	return ContourFound;
}


void __attribute__((section(".fast_code_sec"))) Proc_ContourSegmentation(struct Proc_ContourStruct* Contour)
{
	uint32_t I;
	uint32_t X, Y;
	uint8_t* Directions;
	uint32_t Direction;
	uint32_t DirType, PrevDirType;

	// Do segmentation for current contour
	Proc_ContourFreeX = 0;
	for (I=0; I<L; I++)
		Proc_ContourSegmentsY[I] = 0xFFFF;

	X = Contour->XStart;
	Y = Contour->YStart;
	Directions	= Contour->Directions;
	PrevDirType = Proc_DirectionType(Directions[(Contour->PointsNumber)-1]);

	// Create segments from contour
	for (I=0; I<(Contour->PointsNumber); I++)
	{
		Direction = *Directions++;
		DirType	  = Proc_DirectionType(Direction);
		// double store X (a peak) to make interval [X, X]
		if ((DirType + PrevDirType) == (PROC_DIR_TYPE_BEGIN + PROC_DIR_TYPE_END))
		{
			Proc_StorePointX(X, Y);
			Proc_StorePointX(X, Y);
		}
		else
		{
			if (((DirType == PROC_DIR_TYPE_HPLUS)  && (PrevDirType >= PROC_DIR_TYPE_HMINUS)) ||
			    ((DirType == PROC_DIR_TYPE_HMINUS) && (PrevDirType <= PROC_DIR_TYPE_END))    ||
			    ((DirType == PROC_DIR_TYPE_BEGIN)  && (PrevDirType >= PROC_DIR_TYPE_HMINUS)) ||
			    ((DirType == PROC_DIR_TYPE_END)    && (PrevDirType <= PROC_DIR_TYPE_END)))
				Proc_StorePointX(X, Y); // store point
		}

		PrevDirType = DirType;
		X += Proc_DirectionAddX[Direction];
		Y += Proc_DirectionAddY[Direction];
	}

	// Optimize segments (concatenate segments if Seg1Xend = Seg2Xstart) - remove double defined points
	uint32_t ContourX, ContourXStart;
	for (Y = Contour->YMin; Y<(Contour->YMin + Contour->YSize); Y++)
	{
		ContourX = Proc_ContourSegmentsY[Y];
		while (1)
		{
			ContourXStart = ContourX;
			ContourX = Proc_ContourSegmentsX[ContourX].NextX;
			X = Proc_ContourSegmentsX[ContourX].X;
			ContourX = Proc_ContourSegmentsX[ContourX].NextX;
			if (ContourX>>15)
				break;
			if (X == Proc_ContourSegmentsX[ContourX].X)
			{
				Proc_ContourSegmentsX[ContourXStart].NextX = Proc_ContourSegmentsX[ContourX].NextX;
				ContourX = ContourXStart;
			}
		}
	}
}


void __attribute__((section(".fast_code_sec"))) Proc_BuildMaskFromContourSegments(struct Proc_ContourStruct* Contour, uint32_t FillType)
{
	int32_t  X, XEnd, Y;
	int32_t  Index;
	uint32_t ContourX;

	if (FillType == PROC_FILL_MASK_ALL)
	{
		for (Y = Contour->YMin; Y<(Contour->YMin + Contour->YSize); Y++)
		{
			ContourX = Proc_ContourSegmentsY[Y];
			do
			{
				X	  = Proc_ContourSegmentsX[ContourX].X;
				ContourX = Proc_ContourSegmentsX[ContourX].NextX;
				XEnd  = Proc_ContourSegmentsX[ContourX].X;
				ContourX = Proc_ContourSegmentsX[ContourX].NextX;

				// Get the mask between X segment
				for (Index = Y*L + X; Index <= (Y*L + XEnd); Index++)
					Proc_ImgMask[Index>>3] |= (1 << (Index & 0x07));
			} while (ContourX != 0xFFFF);
		}
	}
	else
	{
		for (Y = Contour->YMin; Y<(Contour->YMin + Contour->YSize); Y++)
		{
			ContourX = Proc_ContourSegmentsY[Y];
			do
			{
				X	  = Proc_ContourSegmentsX[ContourX].X;
				ContourX = Proc_ContourSegmentsX[ContourX].NextX;
				XEnd  = Proc_ContourSegmentsX[ContourX].X;
				ContourX = Proc_ContourSegmentsX[ContourX].NextX;

				// Get the mask between X segment
				for (Index = Y*L + X; Index <= (Y*L + XEnd); Index++)
					Proc_ImgMask[Index>>3] |= Proc_ImgMaskSec[Index>>3] & (1 << (Index & 0x07));
			} while (ContourX != 0xFFFF);
		}
	}
}


void __attribute__((section(".fast_code_sec"))) Proc_RemoveThinContourBranches(struct Proc_ContourStruct* Contour)
{
	uint32_t I, J, K;
	uint8_t* Directions;
	uint8_t  Direction, PrevDirection;
	int32_t  Index, IndexTmp;
	int32_t  X, Y;
	int32_t	 DeltaX, DeltaY;
	int32_t  Xm1, Xp1, Ym1, Yp1;
	int32_t  SumX, SumY;

	X = Contour->XStart;
	Y = Contour->YStart;
	Directions    = Contour->Directions;
	PrevDirection = Directions[(Contour->PointsNumber)-1];
	Xm1 = X - Proc_DirectionAddX[PrevDirection];
	Ym1 = Y - Proc_DirectionAddY[PrevDirection];

	for (I=0; I<(Contour->PointsNumber-1); I++)
	{
		Direction = *Directions++;

		Xp1 = X + Proc_DirectionAddX[Direction];
		Yp1 = Y + Proc_DirectionAddY[Direction];

		// Compute the perpendicular equation towards inner contour
		DeltaX = Yp1 - Ym1;
		DeltaY = Xm1 - Xp1;
		if ((DeltaX*DeltaX + DeltaY*DeltaY) <= 2)	{DeltaX <<= 1; DeltaY <<= 1;}

		Index = Y*L + X;
		// Check for 7 points toward inner contour if there are holes
		for (SumX=0,SumY=0,J=0; J<8; J++)
		{
			SumX += DeltaX;
			SumY += DeltaY;
			IndexTmp = Index + (SumY>>1)*L + (SumX>>1);
			if ((Proc_ImgMaskSec[IndexTmp>>3] & (1 << (IndexTmp & 0x07))) == 0)
			{
				Proc_ImgMask[Index>>3] &= ~(1 << (Index & 0x07));
				for (SumX=0,SumY=0,K=0; K<=J; K++)
				{
					SumX += DeltaX;
					SumY += DeltaY;
					IndexTmp = Index + (SumY>>1)*L + (SumX>>1);
					Proc_ImgMask[IndexTmp>>3] &= ~(1 << (IndexTmp & 0x07));
				}
				break;
			}
		}
		Xm1 = X;
		Ym1 = Y;
		X 	= Xp1;
		Y 	= Yp1;
	}
}


void __attribute__((section(".fast_code_sec"))) Proc_GetMaximumSegment(int32_t Y, int32_t* XMin, int32_t* XMax)
{
	int32_t X;
	int32_t XStart = -1;
	int32_t SegMax = 0;
	int32_t Seg;
	uint32_t Index = Y*Proc_NormalizedMaskWidth;

	for (X=0; X<Proc_NormalizedMaskWidth; X++)
		if (Proc_GetMaskLinearNormalized(Index++))
			XStart = (XStart < 0) ?	X:XStart;
		else
			if (XStart >= 0)
			{
				Seg = X-XStart;
				if (Seg>SegMax)	{SegMax= Seg; *XMin = XStart; *XMax = X-1;}
				XStart = -1;
			}
	if (XStart >= 0) {*XMin = XStart; *XMax = X-1;}
}


void __attribute__((section(".fast_code_sec"))) Proc_GetExtremeSegment(int32_t Y, int32_t* XMin, int32_t* XMax)
{
	int32_t X;
	uint32_t Index = Y*Proc_NormalizedMaskWidth;

	*XMin = -1;
	for (X=0; X<Proc_NormalizedMaskWidth; X++)
		if (Proc_GetMaskLinearNormalized(Index++))
		{
			if (*XMin < 0) *XMin = X;
			*XMax = X;
		}
}


void __attribute__((section(".fast_code_sec"))) Proc_FillSegmentsData(uint32_t SegNumber, uint32_t SegType, int32_t YMin, int32_t YMax)
{
	int32_t XMin	= 0;
	int32_t XMax	= 0;
	int32_t XSize;
	int32_t Sum 	= 0;
	int32_t MinPos	= 0;
	int32_t	MinSize = 0xFFFF;
	int32_t MaxPos	= 0;
	int32_t MaxSize = 0;


	for (int32_t Y=YMin; Y<=YMax; Y++)
	{
		if (SegType == 0)	Proc_GetMaximumSegment(Y, &XMin, &XMax);
		else				Proc_GetExtremeSegment(Y, &XMin, &XMax);
		XSize = XMax - XMin + 1;
		if (XSize<MinSize)	{MinSize=XSize; MinPos = Y;}
		if (XSize>MaxSize)	{MaxSize=XSize; MaxPos = Y;}
		Sum	+= XSize;
	}
	Proc_IntervalSegmentData[SegNumber].MinPos	= MinPos;
	Proc_IntervalSegmentData[SegNumber].MinSize	= MinSize;
	Proc_IntervalSegmentData[SegNumber].MaxPos	= MaxPos;
	Proc_IntervalSegmentData[SegNumber].MaxSize	= MaxSize;
	Proc_IntervalSegmentData[SegNumber].AvgSize = Sum/(YMax-YMin+1);
}


void __attribute__((section(".fast_code_sec"))) Proc_ComputeWingsLengthAngle()
{
	int32_t  LeftWing  = -1;
	int32_t  RightWing = -1;
	int32_t  X, Y;
	int32_t  XDist, YDist, Dist;
	int32_t  LeftXDist, LeftYDist, RightXDist, RightYDist;
	uint32_t Index;

	LeftXDist=LeftYDist=RightXDist=RightYDist=0;
	// Check the wing length after 1/8 of the body length (avoid head)
	for (Y=Proc_HeadSegmentYCenter + (Proc_NormalizedMaskHeight/8); Y<Proc_NormalizedMaskHeight;Y++)
	{
		// Check left wing
		Index		= Y*Proc_NormalizedMaskWidth;
		for (X=0; X<(Proc_HeadSegmentXCenter/2); X++)
			if (Proc_GetMaskLinearNormalized(Index++))
			{
				XDist = Proc_HeadSegmentXCenter - X + 1;
				YDist = Y - Proc_HeadSegmentYCenter + 1;
				Dist = (int32_t)sqrt(XDist*XDist + YDist*YDist);
				if (Dist>LeftWing) {LeftWing = Dist;	LeftXDist = XDist;	LeftYDist = YDist;}
				break;
			}
		// Check right wing
		Index = (Y+1)*Proc_NormalizedMaskWidth - 1;
		for (X=Proc_NormalizedMaskWidth-1; X>(Proc_HeadSegmentXCenter/2); X--)
			if (Proc_GetMaskLinearNormalized(Index--))
			{
				XDist = X - Proc_HeadSegmentXCenter + 1;
				YDist = Y - Proc_HeadSegmentYCenter + 1;
				Dist = (int32_t)sqrt(XDist*XDist + YDist*YDist);
				if (Dist>RightWing) {RightWing = Dist;	RightXDist = XDist;	RightYDist = YDist;}
				break;
			}
	}

	Temp[0] = (double)LeftXDist;
	Temp[1] = (double)LeftYDist;
	Temp[2] = (double)RightXDist;
	Temp[3] = (double)RightYDist;

	Proc_WingLength[0] = LeftWing;
	Proc_WingLength[1] = RightWing;
	Proc_WingAngle = atan(((double)LeftXDist)/LeftYDist) + atan(((double)RightXDist)/RightYDist);
}


void __attribute__((section(".fast_code_sec"))) Proc_ComputeAvgObjectColor(struct Proc_ContourStruct* Contour)
{
	int32_t  X, XEnd, Y;
	uint32_t ContourX;
	uint8_t* Src00 = (uint8_t*)Utils_GetDemosaicImgPtr();
	uint8_t* SrcY;
	uint8_t* SrcXY;
	uint32_t R = 0;
	uint32_t G = 0;
	uint32_t B = 0;

	for (Y = Contour->YMin; Y<(Contour->YMin + Contour->YSize); Y++)
	{
		SrcY = Src00 + (Y - Proc_YMin)*Proc_SizeX*3;
		ContourX = Proc_ContourSegmentsY[Y];
		do
		{
			X	  = Proc_ContourSegmentsX[ContourX].X;
			ContourX = Proc_ContourSegmentsX[ContourX].NextX;
			XEnd  = Proc_ContourSegmentsX[ContourX].X;
			ContourX = Proc_ContourSegmentsX[ContourX].NextX;
			// Compute address from image
			SrcXY = SrcY + (X - Proc_XMin)*3;

			// Get the mask between X segment
			for (; X <= XEnd; X++)
			{
				B += *SrcXY++;
				G += *SrcXY++;
				R += *SrcXY++;
			}
		} while (ContourX != 0xFFFF);
	}
	B /= Contour->ExtraData->PointsArea;
	G /= Contour->ExtraData->PointsArea;
	R /= Contour->ExtraData->PointsArea;

	Contour->ExtraData->ColorRgbAvg = (R<<16) | (G<<8) | B;
	// Compute the HSL color
	double Hf,Sf,Lf;
	double Rf = (double)R/255;
	double Gf = (double)G/255;
	double Bf = (double)B/255;
	double Max = (Rf>Gf)?Rf:Gf;	Max = (Bf>Max)?Bf:Max;
	double Min = (Rf<Gf)?Rf:Gf;	Min = (Bf<Min)?Bf:Min;
	double C = Max-Min;
	Lf = (Max+Min)/2;
	if (C == 0)		Hf=Sf=0;
	else
	{
		Sf = (Lf>0.5f)?(C/(2-Max-Min)):(C/(Max+Min));
		if (Max == Rf)		Hf = (Gf-Bf)/C + ((Gf<Bf)?(6):(0));
		else if (Max == Gf)	Hf = (Bf-Rf)/C + 2;
			 else			Hf = (Rf-Gf)/C + 4;
	}
	Contour->ExtraData->ColorHAvg = (uint32_t)round(Hf*60);  // convert it to 0..360 degree
	Contour->ExtraData->ColorSAvg = (uint32_t)round(Sf*100); // convert it to 0..100%
	Contour->ExtraData->ColorLAvg = (uint32_t)round(Lf*100); // convert it to 0..100%
}


uint32_t __attribute__((section(".fast_code_sec"))) Proc_ComputeContourParameters(struct Proc_ContourStruct* Contour)
{
	int32_t  X, XEnd, Y;
	uint32_t I, J, Index;
	uint32_t ContourX;
	double	 XFloat;
	double	 YFloat;
	double	 M00;
	uint32_t M00Int	= 0;
	double	 M10 	= 0;
	double	 M01	= 0;
	double	 M11	= 0;
	double	 M20	= 0;
	double	 M02	= 0;
	double	 M21	= 0;
	double	 M12	= 0;
	double	 M30	= 0;
	double	 M03	= 0;
	double	 ObjXCenter, ObjYCenter;

	// Set first insect name as unknown
	strcpy(Contour->ExtraData->Name, "Unknown");
	strcpy(Contour->ExtraData->ShortName, "Unknown");

	// Parse contour segments and extract HU moments and direction axe
	for (Y = Contour->YMin; Y<(Contour->YMin + Contour->YSize); Y++)
	{
		ContourX = Proc_ContourSegmentsY[Y];
		YFloat = (double)Y;
		do
		{
			X	  = Proc_ContourSegmentsX[ContourX].X;
			ContourX = Proc_ContourSegmentsX[ContourX].NextX;
			XEnd  = Proc_ContourSegmentsX[ContourX].X;
			ContourX = Proc_ContourSegmentsX[ContourX].NextX;

			// Compute M00
			M00Int += XEnd - X + 1;
			for (; X <= XEnd; X++)
			{
				// Compute other raw moments
				XFloat = (double)X;
				M10 += XFloat;
				M01 += YFloat;
				M11 += XFloat*YFloat;
				M20 += XFloat*XFloat;
				M02 += YFloat*YFloat;
				M21 += XFloat*XFloat*YFloat;
				M12 += XFloat*YFloat*YFloat;
				M30 += XFloat*XFloat*XFloat;
				M03 += YFloat*YFloat*YFloat;
			}
		}while (ContourX != 0xFFFF);
	}

	Contour->ExtraData->PointsArea = M00Int;
	M00 = (double)M00Int;
	// Compute center of the object
	ObjXCenter = M10/M00;
	ObjYCenter = M01/M00;
	Contour->ExtraData->XCenter = ObjXCenter;
	Contour->ExtraData->YCenter = ObjYCenter;

	double Y00 = M00;
	double Y11 = M11 - ObjXCenter*M01;
	double Y20 = M20 - ObjXCenter*M10;
	double Y02 = M02 - ObjYCenter*M01;
	double Y21 = M21 - 2*ObjXCenter*M11 - ObjYCenter*M20 + 2*ObjXCenter*ObjXCenter*M01;
	double Y12 = M12 - 2*ObjYCenter*M11 - ObjXCenter*M02 + 2*ObjYCenter*ObjYCenter*M10;
	double Y30 = M30 - 3*ObjXCenter*M20 + 2*ObjXCenter*ObjXCenter*M10;
	double Y03 = M03 - 3*ObjYCenter*M02 + 2*ObjYCenter*ObjYCenter*M01;

	double Y00_2_0 = Y00 * Y00;
	double Y00_2_5 = Y00_2_0 * sqrt(Y00);

	double E11 = Y11 / Y00_2_0;
	double E20 = Y20 / Y00_2_0;
	double E02 = Y02 / Y00_2_0;
	double E21 = Y21 / Y00_2_5;
	double E12 = Y12 / Y00_2_5;
	double E30 = Y30 / Y00_2_5;
	double E03 = Y03 / Y00_2_5;

	/* Compute HU moments */
	Contour->ExtraData->HuMoment[0] = E20 + E02;
	Contour->ExtraData->HuMoment[1] = (E20 - E02)*(E20 - E02) + 4*E11*E11;
	Contour->ExtraData->HuMoment[2] = (E30 - 3*E12)*(E30 - 3*E12) + (3*E21 - E03)*(3*E21 - E03);
	Contour->ExtraData->HuMoment[3] = (E30 + E12)*(E30 + E12) + (E21 + E03)*(E21 + E03);
	Contour->ExtraData->HuMoment[4] = (E30 - 3*E12)*(E30 + E12)*((E30 + E12)*(E30 + E12) - 3*(E21 + E03)*(E21 + E03)) +
									  (3*E21 - E03)*(E21 + E03)*(3*(E30 + E12)*(E30 + E12) - (E21 + E03)*(E21 + E03));
	Contour->ExtraData->HuMoment[5] = (E20 - E02)*((E30 + E12)*(E30 + E12) - (E21 + E03)*(E21 + E03)) + 4*E11*(E30 + E12)*(E21 + E03);
	Contour->ExtraData->HuMoment[6] = (3*E21 - E03)*(E30 + E12)*((E30 + E12)*(E30 + E12) - 3*(E21 + E03)*(E21 + E03)) -
									  (E30 - 3*E12)*(E21 + E03)*(3*(E30 + E12)*(E30 + E12) - (E21 + E03)*(E21 + E03));

	// Log10 all Hu moments to normalize scale
	for (I=0; I<7; I++)
		if (Contour->ExtraData->HuMoment[I] > 0.0F)		Contour->ExtraData->HuMoment[I] =  log10(Contour->ExtraData->HuMoment[I]);
		else											Contour->ExtraData->HuMoment[I] = -log10(-Contour->ExtraData->HuMoment[I]);

	/* Compute the object orientation */
	double Yp11 = Y11 / Y00;
	double Yp02 = Y02 / Y00;
	double Yp20 = Y20 / Y00;

	double TmpFloat = (2*Yp11)/(Yp20-Yp02);
	double ObjAngle;
	if (isinf(TmpFloat))
		ObjAngle = (Yp11 > 0) ? (M_PI_4 + M_PI_2):(M_PI_4);
	else
	{
		ObjAngle = atan(TmpFloat)/2;
		if ((Yp20-Yp02) < 0)	ObjAngle  = M_PI_2 - ObjAngle;	// Close to Y axe
		else					ObjAngle  = M_PI   - ObjAngle;	// Close to X axe
		if (ObjAngle >= M_PI)	ObjAngle -= M_PI;
	}

	//Compute Length and Width by rotating all the contour points
	double Xf, Yf;
	double XRotate, YRotate;
	double ObjAngleSin = sin(M_PI_2 - ObjAngle);
	double ObjAngleCos = cos(M_PI_2 - ObjAngle);
	double XMin =  DBL_MAX;
	double XMax = -DBL_MAX;
	double YMin =  DBL_MAX;
	double YMax = -DBL_MAX;
	uint8_t* Directions = Contour->Directions;
	uint8_t  Direction;
	double Perimeter=0;

	X = Contour->XStart;
	Y = Contour->YStart;

	for (uint32_t I=Contour->PointsNumber; I>0; I--)
	{
		Xf = (double)X - ObjXCenter;
		Yf = (double)Y - ObjYCenter;
		// Rotate X, Y
		XRotate = Yf*ObjAngleSin + Xf*ObjAngleCos;
		YRotate = Yf*ObjAngleCos - Xf*ObjAngleSin;
		XMin = (XRotate < XMin) ? XRotate:XMin;
		XMax = (XRotate > XMax) ? XRotate:XMax;
		YMin = (YRotate < YMin) ? YRotate:YMin;
		YMax = (YRotate > YMax) ? YRotate:YMax;
		Direction = *Directions++;
		Perimeter += Proc_PerimeterAdd[Direction];
		X += Proc_DirectionAddX[Direction];
		Y += Proc_DirectionAddY[Direction];
	}
	Contour->ExtraData->Perimeter	= Perimeter;
	Contour->ExtraData->Compactness	= (4*M_PI*(Contour->ExtraData->PointsArea))/(Perimeter*Perimeter);
	XMin += ObjXCenter;
	XMax += ObjXCenter;
	YMin += ObjYCenter;
	YMax += ObjYCenter;
	Contour->ExtraData->Length		= (YMax - YMin)*NC_PIXEL_IN_MM;
	Contour->ExtraData->Width		= (XMax - XMin)*NC_PIXEL_IN_MM;


	//Rotate the whole mask
	int32_t Width 	= (int32_t)round(XMax) - (int32_t)round(XMin) + 1;
	int32_t Width8 	= (Width + 8) & 0xFFF8;
	int32_t Height 	= (int32_t)round(YMax) - (int32_t)round(YMin) + 1;
	Proc_NormalizedMaskWidth  = Width8;
	Proc_NormalizedMaskHeight = Height;

	if ((Proc_NormalizedMaskWidth*Proc_NormalizedMaskHeight) > ((L*L*NC_MAX_CONTOUR_IMAGE_PERCENTAGE)/100))
		return 0;

	double  XShift	= ObjXCenter - XMin;
	double  YShift  = ObjYCenter - YMin;
	double  XAdd, YAdd;

	// Clear mask (prepare it for current contour)
	Proc_ClearMaskAll(Proc_ImgMaskNormalized);
	// Rotate mask and shift it to (0,0) coordinate
	for (Y = Contour->YMin; Y<(Contour->YMin + Contour->YSize); Y++)
	{
		Yf = (double)Y - ObjYCenter;
		XAdd = XShift + Yf*ObjAngleSin;
		YAdd = YShift + Yf*ObjAngleCos;
		ContourX = Proc_ContourSegmentsY[Y];
		do
		{
			X	  = Proc_ContourSegmentsX[ContourX].X;
			ContourX = Proc_ContourSegmentsX[ContourX].NextX;
			XEnd  = Proc_ContourSegmentsX[ContourX].X;
			ContourX = Proc_ContourSegmentsX[ContourX].NextX;

			// Get the mask between X segment
			for (; X <= XEnd; X++)
			{
				Xf = (double)X - ObjXCenter;
				XRotate = XAdd + Xf*ObjAngleCos;
				YRotate = YAdd - Xf*ObjAngleSin;
				Index = ((int32_t)round(YRotate))*Width8 + (int32_t)round(XRotate);
				Proc_ImgMaskNormalized[Index>>3] |= (1 << (Index & 0x07));
			}
		} while (ContourX != 0xFFFF);
	}

	//Fill the small gaps appearing after rotation
	uint32_t Sum;
	for (Y=(Height-2), Index=(Width8+1); Y>0; Y--, Index+=2)
		for (X=Width8-2; X>0; X--, Index++)
			if (Proc_GetMaskLinearNormalized(Index) == 0)
			{
				Sum = 0;
				if (Proc_GetMaskLinearNormalized(Index-1))				Sum++;
				if (Proc_GetMaskLinearNormalized(Index+1))				Sum++;
				if (Proc_GetMaskLinearNormalized(Index-Width8-1))		Sum++;
				if (Proc_GetMaskLinearNormalized(Index-Width8))			Sum++;
				if (Sum == 0) 											continue;
				if (Proc_GetMaskLinearNormalized(Index-Width8+1))		Sum++;
				if (Proc_GetMaskLinearNormalized(Index+Width8-1))		Sum++;
				if (Proc_GetMaskLinearNormalized(Index+Width8))			Sum++;
				if (Proc_GetMaskLinearNormalized(Index+Width8+1))		Sum++;
				if (Sum >= 5)											Proc_ImgMaskNormalized[Index>>3] |= (1 << (Index & 0x07));
			}

	// Check left/right symmetry
	int32_t  X1, X2, Y1, Y2;
	int32_t Index1, Index2, Index1Min, Index2Max;
	uint32_t A, B;
	uint32_t SamePx, DiffPx;
	uint32_t RatioLeftRight, RatioUpDown;

	X2 = (int32_t)round(XShift);
	X1 = X2 - 1;
	X  = X2;
	if  (X < (Width - X)) 	X = Width - X;

	SamePx = DiffPx = 0;
	for (Y=0; Y<Height; Y++)
	{
		Index1    = Y*Width8 + X1;
		Index2    = Y*Width8 + X2;
		Index1Min = Y*Width8;
		Index2Max = Y*Width8 + Width - 1;

		for (I=0; I<X; I++)
		{
			A = B = 0;
			if (Index1 >= Index1Min)			A = Proc_GetMaskLinearNormalized(Index1--);
			if (Index2 <= Index2Max)			B = Proc_GetMaskLinearNormalized(Index2++);
			if ((A && (!B)) || ((!A) && B))		DiffPx++;
			if (A && B)							SamePx++;
		}
	}
	if (DiffPx)	RatioLeftRight = (SamePx<<8)/DiffPx;
	else		RatioLeftRight = 0xFFFFFFFF;

	// Check up/down symmetry
	Y2 = (int32_t)round(YShift);
	Y1 = Y2 - 1;
	Y  = Y2;
	if  (Y < (Height - Y)) 	Y = Height - Y;

	SamePx = DiffPx = 0;
	for (; Y>0; Y--)
	{
		Index1  = (Y1--)*Width8;
		Index2  = (Y2++)*Width8;
		Index1Min = 0;
		Index2Max = Height*Width8 - 1;
		for (I=0; I<Width; I++)
		{
			A = B = 0;
			if (Index1 >= Index1Min)			A = Proc_GetMaskLinearNormalized(Index1++);
			if (Index2 <= Index2Max)			B = Proc_GetMaskLinearNormalized(Index2++);
			if ((A && (!B)) || ((!A) && B))		DiffPx++;
			if (A && (B))						SamePx++;
		}
	}
	if (DiffPx)	RatioUpDown = (SamePx<<8)/DiffPx;
	else		RatioUpDown = 0xFFFFFFFF;


	// If up/down symmetry is better than left/right symmetry then the object major direction axe will be rotated +90 degree
	// Swap Length with Width too
	// Usually the center of mass of insects toward tail; update direction accordingly
	uint32_t ReRotate = 0;
	if (((Height/Width)<2) && (RatioUpDown>RatioLeftRight))
	{
		ObjAngle += M_PI/2;
		if (XShift < (Width/2))		ObjAngle += M_PI;
		if (ObjAngle >= 2*M_PI)		ObjAngle -= 2*M_PI;
		ReRotate = 1;
	}
	else
		if (YShift < (Height/2))	{ ObjAngle += M_PI; ReRotate = 1;}

	Contour->ExtraData->Angle 	 = (ObjAngle);
	Contour->ExtraData->AngleDeg = (ObjAngle*180)/M_PI;

	// Re-rotate image with new angles
	if (ReRotate)
	{
		ObjAngleSin = sin(M_PI_2 - ObjAngle);
		ObjAngleCos = cos(M_PI_2 - ObjAngle);
		XMin =  DBL_MAX;
		XMax = -DBL_MAX;
		YMin =  DBL_MAX;
		YMax = -DBL_MAX;
		Directions = Contour->Directions;

		X = Contour->XStart;
		Y = Contour->YStart;

		for (uint32_t I=Contour->PointsNumber; I>0; I--)
		{
			Xf = (double)X - ObjXCenter;
			Yf = (double)Y - ObjYCenter;
			// Rotate X, Y
			XRotate = Yf*ObjAngleSin + Xf*ObjAngleCos;
			YRotate = Yf*ObjAngleCos - Xf*ObjAngleSin;
			XMin = (XRotate < XMin) ? XRotate:XMin;
			XMax = (XRotate > XMax) ? XRotate:XMax;
			YMin = (YRotate < YMin) ? YRotate:YMin;
			YMax = (YRotate > YMax) ? YRotate:YMax;
			Direction = *Directions++;
			X += Proc_DirectionAddX[Direction];
			Y += Proc_DirectionAddY[Direction];
		}
		XMin += ObjXCenter;
		XMax += ObjXCenter;
		YMin += ObjYCenter;
		YMax += ObjYCenter;
		Contour->ExtraData->Length = (YMax - YMin)*NC_PIXEL_IN_MM;
		Contour->ExtraData->Width  = (XMax - XMin)*NC_PIXEL_IN_MM;

		//Rotate the whole mask
		Width 	= (int32_t)round(XMax) - (int32_t)round(XMin) + 1;
		Width8 	= (Width + 8) & 0xFFF8;
		Height 	= (int32_t)round(YMax) - (int32_t)round(YMin) + 1;
		Proc_NormalizedMaskWidth  = Width8;
		Proc_NormalizedMaskHeight = Height;
		XShift	= ObjXCenter - XMin;
		YShift  = ObjYCenter - YMin;

		// Clear mask (prepare it for current contour)
		Proc_ClearMaskAll(Proc_ImgMaskNormalized);
		// Rotate mask and shift it to (0,0) coordinate
		for (Y = Contour->YMin; Y<(Contour->YMin + Contour->YSize); Y++)
		{
			Yf = (double)Y - ObjYCenter;
			XAdd = XShift + Yf*ObjAngleSin;
			YAdd = YShift + Yf*ObjAngleCos;
			ContourX = Proc_ContourSegmentsY[Y];
			do
			{
				X	  = Proc_ContourSegmentsX[ContourX].X;
				ContourX = Proc_ContourSegmentsX[ContourX].NextX;
				XEnd  = Proc_ContourSegmentsX[ContourX].X;
				ContourX = Proc_ContourSegmentsX[ContourX].NextX;

				// Get the mask between X segment
				for (; X <= XEnd; X++)
				{
					Xf = (double)X - ObjXCenter;
					XRotate = XAdd + Xf*ObjAngleCos;
					YRotate = YAdd - Xf*ObjAngleSin;
					Index = ((int32_t)round(YRotate))*Width8 + (int32_t)round(XRotate);
					Proc_ImgMaskNormalized[Index>>3] |= (1 << (Index & 0x07));
				}
			} while (ContourX != 0xFFFF);
		}

		//Fill the small gaps appearing after rotation
		for (Y=(Height-2), Index=(Width8+1); Y>0; Y--, Index+=2)
			for (X=Width8-2; X>0; X--, Index++)
				if (Proc_GetMaskLinearNormalized(Index) == 0)
				{
					Sum = 0;
					if (Proc_GetMaskLinearNormalized(Index-1))				Sum++;
					if (Proc_GetMaskLinearNormalized(Index+1))				Sum++;
					if (Proc_GetMaskLinearNormalized(Index-Width8-1))		Sum++;
					if (Proc_GetMaskLinearNormalized(Index-Width8))			Sum++;
					if (Sum == 0) 											continue;
					if (Proc_GetMaskLinearNormalized(Index-Width8+1))		Sum++;
					if (Proc_GetMaskLinearNormalized(Index+Width8-1))		Sum++;
					if (Proc_GetMaskLinearNormalized(Index+Width8))			Sum++;
					if (Proc_GetMaskLinearNormalized(Index+Width8+1))		Sum++;
					if (Sum >= 5)											Proc_ImgMaskNormalized[Index>>3] |= (1 << (Index & 0x07));
				}
	}

	if (C_ActivateNormMaskOverImage)
		Proc_AddNormMaskOverPicture(Width8, Height);

	Proc_ComputeAvgObjectColor(Contour);

	//Check if the insect corresponds to the valid ones
	const struct Proc_InsectModelStruct* InsectsModel;
	for (I=0; I<Config_NumberOfActiveInsects; I++)
	{
		InsectsModel = &C_InsectsModels[Config_ActiveInsectsIdx[I]];
		// Check primary ranges if they fit (size, color, Hu Moments)
		if 	   (((Contour->ExtraData->Length)    < (InsectsModel->LengthRange[0])) ||
				((Contour->ExtraData->Length)    > (InsectsModel->LengthRange[1])) ||
				((Contour->ExtraData->Width)	 < (InsectsModel->WidthRange[0])) ||
				((Contour->ExtraData->Width)     > (InsectsModel->WidthRange[1])) ||
				((Contour->ExtraData->ColorHAvg) < (InsectsModel->ColorHueRange[0])) ||
				((Contour->ExtraData->ColorHAvg) > (InsectsModel->ColorHueRange[1])) ||
				((Contour->ExtraData->ColorSAvg) < (InsectsModel->ColorSatRange[0])) ||
				((Contour->ExtraData->ColorSAvg) > (InsectsModel->ColorSatRange[1])) ||
				((Contour->ExtraData->ColorLAvg) < (InsectsModel->ColorLumRange[0])) ||
				((Contour->ExtraData->ColorLAvg) > (InsectsModel->ColorLumRange[1])) ||
				((Contour->ExtraData->HuMoment[0]) < (InsectsModel->HuMomentRange[0][0])) ||
				((Contour->ExtraData->HuMoment[0]) > (InsectsModel->HuMomentRange[0][1])) ||
				((Contour->ExtraData->HuMoment[1]) < (InsectsModel->HuMomentRange[1][0])) ||
				((Contour->ExtraData->HuMoment[1]) > (InsectsModel->HuMomentRange[1][1])) ||
				((Contour->ExtraData->HuMoment[2]) < (InsectsModel->HuMomentRange[2][0])) ||
				((Contour->ExtraData->HuMoment[2]) > (InsectsModel->HuMomentRange[2][1])) ||
				((Contour->ExtraData->HuMoment[3]) < (InsectsModel->HuMomentRange[3][0])) ||
				((Contour->ExtraData->HuMoment[3]) > (InsectsModel->HuMomentRange[3][1])) ||
				((Contour->ExtraData->HuMoment[4]) < (InsectsModel->HuMomentRange[4][0])) ||
				((Contour->ExtraData->HuMoment[4]) > (InsectsModel->HuMomentRange[4][1])) ||
				((Contour->ExtraData->HuMoment[5]) < (InsectsModel->HuMomentRange[5][0])) ||
				((Contour->ExtraData->HuMoment[5]) > (InsectsModel->HuMomentRange[5][1])) ||
				(fabs((Contour->ExtraData->HuMoment[6])) < (InsectsModel->HuMomentRange[6][0])) ||
				(fabs((Contour->ExtraData->HuMoment[6])) > (InsectsModel->HuMomentRange[6][1])))
				continue;

		//Scan for head start
		for (Y=0; Y<Height; Y++)
		{
			Proc_GetMaximumSegment(Y, &X1, &X2);
			if (((X2-X1+1)*NC_PIXEL_IN_MM) >= (InsectsModel->HeadTailMinSize))
			{
				Proc_HeadSegmentYCenter = Y;
				Proc_HeadSegmentXCenter = (X1+X2)/2;
				break;
			}
		}
		if (Y == Height) continue;  // the minimum insect head is not found

		// Check wingspan and wings angle ranges if fly
		if ((InsectsModel->Type) == 1)
		{
			Proc_ComputeWingsLengthAngle();
			double WingSpan		= (double)(Proc_WingLength[0] + Proc_WingLength[1])*NC_PIXEL_IN_MM;
			double WingAngle	= (Proc_WingAngle*180)/M_PI;
			// check wingspan & wingangle
			if ((WingSpan < InsectsModel->WingSpanRange[0]) ||
				(WingSpan > InsectsModel->WingSpanRange[1]) ||
				(WingAngle < InsectsModel->WingAngleRange[0]) ||
				(WingAngle > InsectsModel->WingAngleRange[1]))
				continue;
		}

		// Check segmentation
		if (InsectsModel->NumberOfSegmentScans > 0)
		{
			int32_t YMinInt, YMaxInt;
			for (J=0; J<(InsectsModel->NumberOfSegmentScans); J++)
			{
				YMinInt = (int32_t)((InsectsModel->SegmentScan[J].YMin)*(Proc_NormalizedMaskHeight-Proc_HeadSegmentYCenter)) + Proc_HeadSegmentYCenter;
				YMaxInt = (int32_t)((InsectsModel->SegmentScan[J].YMax)*(Proc_NormalizedMaskHeight-Proc_HeadSegmentYCenter)) + Proc_HeadSegmentYCenter;
				Proc_FillSegmentsData(J, InsectsModel->SegmentScan[J].Type, YMinInt, YMaxInt);
			}
			if (InsectsModel->InsectCheckSegMatch() == 0) continue;
		}

		strcpy(Contour->ExtraData->Name, InsectsModel->Name);
		strcpy(Contour->ExtraData->ShortName, InsectsModel->ShortName);
		break;
	}
	return 1;
}


uint32_t __attribute__((section(".fast_code_sec"))) Proc_RemoveThinContoursBranches(void)
{
	struct Proc_ContourStruct* Contour;

	for (uint32_t C=0; C<Proc_NumberOfContours; C++)
	{
		// Clear mask (prepare it for current contour)
		Proc_ClearMaskAll(Proc_ImgMask);

		Contour = &Proc_Contour[C];
		// Do segmentation for current contour
		Proc_ContourSegmentation(Contour);
		// Build image mask from contour segments
		Proc_BuildMaskFromContourSegments(Contour, PROC_FILL_MASK_ORIGINAL);
		// Parse the contour and cut the thin branches
		Proc_RemoveThinContourBranches(Contour);
		// Update contour after removing small branches (if the contour is to small then remove it)
		if (Proc_UpdateContourAfterRemovals(Contour) == 0)
		{
			for (uint32_t Ctmp = C; Ctmp<(Proc_NumberOfContours-1); Ctmp++)
				Proc_Contour[Ctmp] = Proc_Contour[Ctmp+1];
			Proc_NumberOfContours--;
			C--;
		}
	}
	return Proc_NumberOfContours;
}


uint32_t __attribute__((section(".fast_code_sec"))) Proc_AnalyzeContours(void)
{
	struct Proc_ContourStruct* Contour;

	for (uint32_t C=0; C<Proc_NumberOfContours; C++)
	{
		// Clear mask (prepare it for current contour)
		Proc_ClearMaskAll(Proc_ImgMask);

		Contour = &Proc_Contour[C];
		// Do segmentation for current contour
		Proc_ContourSegmentation(Contour);
		// Build image mask from contour segments
		Proc_BuildMaskFromContourSegments(Contour, PROC_FILL_MASK_ORIGINAL);
		Contour->ExtraData = &Proc_ContourExtraData[C];
		if (Proc_ComputeContourParameters(Contour) == 0)
			return 0;
	}
	return Proc_NumberOfContours;
}



uint32_t __attribute__((section(".fast_code_sec"))) Proc_CheckSegDiabroticaVirgifera(void)
{
	if (((((double)Proc_IntervalSegmentData[0].AvgSize)/Proc_IntervalSegmentData[2].AvgSize) <= 0.80F) &&
		((((double)Proc_IntervalSegmentData[1].AvgSize)/Proc_IntervalSegmentData[2].AvgSize) <= 0.80F))
		return 1;
	return 0;
}


uint32_t Proc_CheckSegHelicoverpaArmigera(void)
{
	if ((Proc_IntervalSegmentData[0].AvgSize >= Proc_IntervalSegmentData[1].AvgSize) ||
	   (Proc_IntervalSegmentData[1].AvgSize >= Proc_IntervalSegmentData[2].AvgSize))
		return 0;
	double R =  ((double)Proc_IntervalSegmentData[0].AvgSize * (double)Proc_IntervalSegmentData[2].AvgSize) /
				((double)Proc_IntervalSegmentData[1].AvgSize * (double)Proc_IntervalSegmentData[1].AvgSize);
	if ((R < 0.8F) || (R > 1.2F))
		return 0;
	return 1;
}


uint32_t Proc_CheckSegOstriniaNubilalis(void)
{
	if ((Proc_IntervalSegmentData[0].AvgSize >= Proc_IntervalSegmentData[1].AvgSize) ||
	   (Proc_IntervalSegmentData[1].AvgSize >= Proc_IntervalSegmentData[2].AvgSize))
		return 0;
	double R =  ((double)Proc_IntervalSegmentData[0].AvgSize * (double)Proc_IntervalSegmentData[2].AvgSize) /
				((double)Proc_IntervalSegmentData[1].AvgSize * (double)Proc_IntervalSegmentData[1].AvgSize);
	if ((R < 0.8F) || (R > 1.2F))
		return 0;
	return 1;
}


uint32_t Proc_CheckSegLobesiaBotrana(void)
{
	if ((Proc_IntervalSegmentData[0].AvgSize >= Proc_IntervalSegmentData[1].AvgSize) ||
	   (Proc_IntervalSegmentData[1].AvgSize >= Proc_IntervalSegmentData[2].AvgSize))
		return 0;
	double R =  ((double)Proc_IntervalSegmentData[0].AvgSize * (double)Proc_IntervalSegmentData[2].AvgSize) /
				((double)Proc_IntervalSegmentData[1].AvgSize * (double)Proc_IntervalSegmentData[1].AvgSize);
	if ((R < 0.8F) || (R > 1.2F))
		return 0;
	return 1;
}

uint32_t Proc_CheckSegHalyomorphaHalys(void)
{
	return 1;
}
