//	==========================================================================
//	calibrations.c
//	(c) 2020, Aurel Dumitru
//
//	Description:
//  Calibrations
//	==========================================================================

// Includes
#include <float.h>
#include "calibrations.h"

// Calibrations
const char     C_HttpAddr[]						= "flytrap.ro";
const char     C_FtpAddr[]						= "ftp.flytrap.ro";
const char     C_FirmwareId[]                   = "2.1.5";

uint32_t	   C_CameraHorizontalPixels			= 960;
uint32_t	   C_CameraVerticalPixels			= 960;

const uint32_t C_ModemBauds[4] = {115200, 921600, 3000000, 3200000};

const struct C_ImsiToApnStruct C_ImsiToApn[32] =
{
		{"22601", "live.vodafone.com"},	//Vodafone Romania
		{"22603", "broadband"},			//Telekom Romania
		{"22605", "internet"},			//Digi Romania
		{"22610", "net"},				//Orange Romania
		{"29505", "em"},				//Emnify
		{"29509", "em"},				//Emnify
		{"53703", "em"},				//Emnify
		{"90143", "em"},				//Emnify
		{"20601", "em"},				//Emnify
		{"23450", "em"},				//Emnify
		{"901",   "iot.1nce.net"},		//1NCE
};
const uint32_t C_ImsiToApnNumber = sizeof(C_ImsiToApn)/sizeof(struct C_ImsiToApnStruct);


const int8_t C_IrDetectionThresholdOffset[] =
{
		/*  0*/ 0,
		/*  1*/ 0,
		/*  2*/ 0,
		/*  3*/ 0,
		/*  4*/ 0,
		/*  5*/ 0,
		/*  6*/ 0,
		/*  7*/ 0,
		/*  8*/ 0,
		/*  9*/ 0,
		/* 10*/ 0,
		/* 11*/ 0,
		/* 12*/ 0,
		/* 13*/ 0,
		/* 14*/ 0,
		/* 15*/ 0,
		/* 16*/ 0,
		/* 17*/ 0,
		/* 18*/ 0,
		/* 19*/ 0,
		/* 20*/ 0,
		/* 21*/ 0,
		/* 22*/ 0,
		/* 23*/ 0,
		/* 24*/ 0,
		/* 25*/ 0,
		/* 26*/ 0,
		/* 27*/ 0,
		/* 28*/ 0,
		/* 29*/ 0,
		/* 30*/ 0,
		/* 31*/ 0,
		/* 32*/ 0,
		/* 33*/ 0,
		/* 34*/ 0,
		/* 35*/ 0,
		/* 36*/ 0,
		/* 37*/ 0,
		/* 38*/ 0,
		/* 39*/ 0,
		/* 40*/ 0,
		/* 41*/ 0,
		/* 42*/ 0,
		/* 43*/ 0,
		/* 44*/ 0,
		/* 45*/ 0,
		/* 46*/ 0,
		/* 47*/ 0,
		/* 48*/ 0,
		/* 49*/ 0,
		/* 50*/ 0,
		/* 51*/ 0,
		/* 52*/ 0,
		/* 53*/ 0,
		/* 54*/ 0,
		/* 55*/ 0,
		/* 56*/ 0,
		/* 57*/ 0,
		/* 58*/ 0,
		/* 59*/ 0,
		/* 60*/ 0,
		/* 61*/ 0,
		/* 62*/ 0,
		/* 63*/ 0,
		/* 64*/ 0,
		/* 65*/ 0,
		/* 66*/ 0,
		/* 67*/ 0,
		/* 68*/ 0,
		/* 69*/ 0,
		/* 70*/ 0,
		/* 71*/ 0,
		/* 72*/ 0,
		/* 73*/ 0,
		/* 74*/ 0,
		/* 75*/ 0,
		/* 76*/ 0,
		/* 77*/ 0,
		/* 78*/ 0,
		/* 79*/ 0,
		/* 80*/ 0,
		/* 81*/ 0,
		/* 82*/ 0,
		/* 83*/ 0,
		/* 84*/ 0,
		/* 85*/ 0,
		/* 86*/ 0,
		/* 87*/ 0,
		/* 88*/ 0,
		/* 89*/ 0,
		/* 90*/ 0,
		/* 91*/ 0,
		/* 92*/ 0,
		/* 93*/ 0,
		/* 94*/ 0,
		/* 95*/ 0,
		/* 96*/ 0,
		/* 97*/ 0,
		/* 98*/ 0,
		/* 99*/ 0,
		/*100*/ 0,
		/*101*/ 0,
		/*102*/ 0,
		/*103*/ 0,
		/*104*/ 0,
		/*105*/ 0,
		/*106*/ 0,
		/*107*/ 0,
		/*108*/ 0,
		/*109*/ 0,
		/*110*/ 0,
		/*111*/ 0,
		/*112*/ 0,
		/*113*/ 0,
		/*114*/ 0,
		/*115*/ 0,
		/*116*/ 0,
		/*117*/ 0,
		/*118*/ 0,
		/*119*/ 0,
		/*120*/ 0,
		/*121*/ 0,
		/*122*/ 0,
		/*123*/ 0,
		/*124*/ 0,
		/*125*/ 0,
		/*126*/ 0,
		/*127*/ 1,
		/*128*/ 1,
		/*129*/ 1,
		/*130*/ 1,
		/*131*/ 1,
		/*132*/ 1,
		/*133*/ 1,
		/*134*/ 1,
		/*135*/ 1,
		/*136*/ 1,
		/*137*/ 1,
		/*138*/ 1,
		/*139*/ 1,
		/*140*/ 1,
		/*141*/ 2,
		/*142*/ 2,
		/*143*/ 2,
		/*144*/ 2,
		/*145*/ 2,
		/*146*/ 2,
		/*147*/ 2,
		/*148*/ 2,
		/*149*/ 2,
		/*150*/ 2,
		/*151*/ 2,
		/*152*/ 2,
		/*153*/ 2,
		/*154*/ 2,
		/*155*/ 2,
		/*156*/ 2,
		/*157*/ 2,
		/*158*/ 2,
		/*159*/ 2,
		/*160*/ 3,
		/*161*/ 3,
		/*162*/ 3,
		/*163*/ 3,
		/*164*/ 3,
		/*165*/ 3,
		/*166*/ 3,
		/*167*/ 3,
		/*168*/ 3,
		/*169*/ 3,
		/*170*/ 4,
		/*171*/ 4,
		/*172*/ 4,
		/*173*/ 4,
		/*174*/ 4,
		/*175*/ 4,
		/*176*/ 4,
		/*177*/ 4,
		/*178*/ 4,
		/*179*/ 4,
		/*180*/ 4,
		/*181*/ 4,
		/*182*/ 4,
		/*183*/ 4,
		/*184*/ 4,
		/*185*/ 4,
		/*186*/ 4,
		/*187*/ 4
};


const struct Proc_InsectSegmentScanStruct C_SegmentScanDiabroticaVirgifera[] =
{	//Type		YMin		YMax
	{1,			0.00F,		0.10F},
	{1,			0.10F,		0.25F},
	{1,			0.45F,		0.80F},

};

const struct Proc_InsectSegmentScanStruct C_SegmentScanHelicoverpaArmigera[] =
{	//Type		YMin		YMax
	{1,			0.25F,		0.40F}, //wing segment 1
	{1,			0.40F,		0.55F}, //wing segment 2
	{1,			0.55F,		0.70F}, //wing segment 3
};

const struct Proc_InsectSegmentScanStruct C_SegmentScanOstriniaNubilalis[] =
{	//Type		YMin		YMax
	{0,			0.25F,		0.40F}, //wing segment 1
	{0,			0.40F,		0.55F}, //wing segment 2
	{0,			0.55F,		0.70F}, //wing segment 3
};

const struct Proc_InsectSegmentScanStruct C_SegmentScanLobesiaBotrana[] =
{	//Type		YMin		YMax
	{0,			0.25F,		0.40F}, //wing segment 1
	{0,			0.40F,		0.55F}, //wing segment 2
	{0,			0.55F,		0.70F}, //wing segment 3
};

const struct Proc_InsectSegmentScanStruct C_SegmentScanHalyomorphaHalys[] =
{	//Type		YMin		YMax
	{0,			0.00F,		1.00F}, //wing segment 1
};


const struct Proc_InsectModelStruct C_InsectsModels[] =
{
	{
		"DiabroticaVirgifera",
		"Dia.Vir.",
		0,							//Bug(0)/Fly(1)
		{5.4F,	7.7F},				//Length
		{1.9F,	3.6F},				//Width
		{0.0F,	0.0F},				//WingsAngle (not used)
		{0.0F,	0.0F},				//WingSpan   (not used)
		{0.27F, 0.51F},				//Compactness
		{36, 51},					//ColorHue
		{28, 44},					//ColorSaturation
		{10, 22},					//ColorLuminance
		{{  -0.61F,		-0.50F},	//HuMoments
		 {  -1.51F,		-1.20F},
		 {  -3.10F,		-2.30F},
		 {  -3.40F,		-2.40F},
		 {  -6.60F,		-4.90F},
		 {  -4.10F,		-3.10F},
		 {   6.30F,		 8.90F}},
		0,							//Patterns
		NULL,
		3,							// 3 segments in scan
		0.3F,						// minimum head size
		C_SegmentScanDiabroticaVirgifera,
		Proc_CheckSegDiabroticaVirgifera
	},
	{
		"HelicoverpaArmigera",
		"Hel.Arm.",
		1,							//Bug(0)/Fly(1)
		{16.3F,	23.1F},				//Length
		{14.2F,	33.0F},				//Width
		{33.0F, 99.0F},				//WingsAngle
		{38.0F,	50.0F},				//WingSpan
		{0.17F, 0.50F},				//Compactness
		{31, 46},					//ColorHue
		{26, 41},					//ColorSaturation
		{17, 50},					//ColorLuminance
		{{  -0.75F,		-0.64F},	//HuMoments
		 {  -4.70F,		-1.89F},
		 {  -2.85F,		-2.00F},
		 {  -5.10F,		-3.70F},
		 {  -9.20F,	     9.20F},
		 {  -7.30F,		 7.30F},
		 {   6.70F,		10.00F}},
		0,							//Patterns
		NULL,
		3,							// 3 segments in scan
		1.0F,						// minimum head size
		C_SegmentScanHelicoverpaArmigera,
		Proc_CheckSegHelicoverpaArmigera
	},
//	{	// Old
//		"HelicoverpaArmigera",
//		"Hel.Arm.",
//		1,							//Bug(0)/Fly(1)
//		{18.0F,	28.0F},				//Length
//		{18.0F,	32.0F},				//Width
//		{33.0F, 95.0F},				//WingsAngle
//		{38.0F,	52.0F},				//WingSpan
//		{0.55F, 0.65F},				//Compactness
//		{0, 320},					//ColorHue
//		{0, 100},					//ColorSaturation
//		{0, 100},					//ColorLuminance
//		{{  -0.74F,		-0.69F},	//HuMoments
//		 {  -5.00F,		-2.00F},
//		 {  -2.90F,		-2.20F},
//		 {  -5.70F,		-4.10F},
//		 { -10.00F,	    10.00F},
//		 { -10.00F,		10.00F},
//		 {   7.50F,		12.00F}},
//		0,							//Patterns
//		NULL,
//		3,							// 3 segments in scan
//		1.0F,						// minimum head size
//		C_SegmentScanHelicoverpaArmigera,
//		Proc_CheckSegHelicoverpaArmigera
//	},
	//	{	// Computed
	//		"OstriniaNubilalis",
	//		"Ost.Nub.",
	//		1,							//Bug(0)/Fly(1)
	//		{12.7F,	17.5F},				//Length
	//		{13.3F,	24.4F},				//Width
	//		{47.0F, 82.0F},				//WingsAngle (usually: 47.0F, 65.0F)
	//		{27.9F,	37.3F},				//WingSpan
	//		{0.28F, 0.56F},				//Compactness
	//		{35, 49},					//ColorHue
	//		{29, 52},					//ColorSaturation
	//		{26, 56},					//ColorLuminance
	//		{{ -0.73758F,		-0.69998F},	//HuMoments
	//		 { -5.56439F,		-2.06931F},
	//		 { -2.58084F,		-2.34543F},
	//		 { -6.85670F,		-4.07497F},
	//		 { -11.63836,	    12.02127F},
	//		 { -8.68291F,		 9.61489F},
	//		 {  7.46361F,		11.80845F}},
	//		0,							//Patterns
	//		NULL,
	//		3,							// 3 segments in scan
	//		1.0F,						// minimum head size
	//		C_SegmentScanOstriniaNubilalis,
	//		Proc_CheckSegOstriniaNubilalis
	//	},
	{
		"OstriniaNubilalis",
		"Ost.Nub.",
		1,							//Bug(0)/Fly(1)
		{12.3F,	17.7F},				//Length
		{12.5F,	22.0F},				//Width
		{40.0F, 83.0F},				//WingsAngle (usually: 47.0F, 65.0F)
		{24.5F,	37.0F},				//WingSpan
		{0.27F, 0.58F},				//Compactness
		{32, 52},					//ColorHue
		{26, 55},					//ColorSaturation
		{24, 57},					//ColorLuminance
		{{ -0.75F,		-0.69F},	//HuMoments
		 { -5.6F,		-2.0F},
		 { -2.7F,		-2.3F},
		 { -7.7F,		-4.0F},
		 { -12.8F,	    12.8F},
		 { -10.2F,		10.2F},
		 {  7.4F,		13.0F}},
		0,							//Patterns
		NULL,
		3,							// 3 segments in scan
		1.0F,						// minimum head size
		C_SegmentScanOstriniaNubilalis,
		Proc_CheckSegOstriniaNubilalis
	},
	{
		"LobesiaBotrana",
		"Lob.Bot.",
		1,							//Bug(0)/Fly(1)
		{ 5.1F,	 8.3F},				//Length
		{ 3.3F,	 6.2F},				//Width
		{20.0F, 60.0F},				//WingsAngle
		{10.0F,	17.5F},				//WingSpan
		{0.35F, 0.65F},				//Compactness
		{26, 48},					//ColorHue
		{22, 40},					//ColorSaturation
		{15, 45},					//ColorLuminance
		{{  -0.75F,		-0.65F},	//HuMoments
		 {  -4.20F,		-1.65F},
		 {  -3.30F,		-2.40F},
		 {  -6.30F,		-3.50F},
		 { -11.00F,	    11.00F},
		 {  -8.00F,		 9.00F},
		 {   7.30F,		11.00F}},
		0,							//Patterns
		NULL,
		3,							// 3 segments in scan
		0.4F,						// minimum head size
		C_SegmentScanLobesiaBotrana,
		Proc_CheckSegLobesiaBotrana
	},
	{
		"HalyomorphaHalys",
		"Hal.Hal.",
		0,							//Bug(0)/Fly(1)
		{12.7F, 17.4F},				//Length
		{ 7.5F,	13.6F},				//Width
		{ 0.0F,  0.0F},				//WingsAngle (not used)
		{ 0.0F,	 0.0F},				//WingSpan (not used)
		{0.29F, 0.58F},				//Compactness
		{25, 46},					//ColorHue
		{17, 31},					//ColorSaturation
		{12, 26},					//ColorLuminance
		{{  -0.77F,		-0.72F},	//HuMoments
		 {  -2.60F,		-2.00F},
		 {  -5.60F,		-3.60F},
		 {  -5.40F,		-4.10F},
		 { -11.00F,	    11.00F},
		 {  -7.50F,		-5.30F},
		 {   8.40F,		12.00F}},
		0,							//Patterns
		NULL,
		1,							// 1 segments in scan
		0.4F,						// minimum head size
		C_SegmentScanHalyomorphaHalys,
		Proc_CheckSegHalyomorphaHalys
	},
};

const uint16_t C_NumberOfInsectsModels = sizeof(C_InsectsModels)/sizeof(C_InsectsModels[0]);

const uint8_t C_DeactivateMotors 			= 0;
const uint8_t C_DeactivateFirmwareCheck 	= 0;
const uint8_t C_ActivateContourDraw     	= 1;
const uint8_t C_DeactivateAutoGainExposure  = 0;
const uint8_t C_ActivateBlueLedOnRain		= 0;
const uint8_t C_ActivateNormMaskOverImage	= 0;
