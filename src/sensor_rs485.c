// ==========================================================================
// sensor_rs485.c
// (c) 2022, Aurel Dumitru
//
// Description:
// RS485 sensors operations
// =========================================================================

#include <string.h>
#include <stdlib.h>
#include "sensor_rs485.h"
#include "main.h"
#include "rtc.h"
#include "devstatus.h"
#include "utils.h"

// Defines
#define SENSOR_RS485_STATE_IDLE		0
#define SENSOR_RS485_STATE_INIT		1

char*	Sensor_RS485_RDPAS01_GetValues(uint32_t Idx, uint32_t Type);
char*	Sensor_RS485_ISRDWO01_GetValues(uint32_t Idx, uint32_t Type);
char*	Sensor_RS485_RDS8IN1_GetValues(uint32_t Idx, uint32_t Type);
char*	Sensor_RS485_SCO201_GetValues(uint32_t Idx, uint32_t Type);
char*	Sensor_RS485_SOR01_GetValues(uint32_t Idx, uint32_t Type);
char*	Sensor_RS485_SC2H4R01_GetValues(uint32_t Idx, uint32_t Type);
char*	Sensor_RS485_RDC2H401_GetValues(uint32_t Idx, uint32_t Type);
char*	Sensor_RS485_RDWSMASA_GetValues(uint32_t Idx, uint32_t Type);
char*	Sensor_RS485_SoilPh_GetValues(uint32_t Idx, uint32_t Type);
char*	Sensor_RS485_SoilTempHumSalEcNpk_GetValues(uint32_t Idx, uint32_t Type);
char*	Sensor_RS485_SoilMoistureLevel2_GetValues(uint32_t Idx, uint32_t Type);
char*	Sensor_RS485_SoilMoistureLevel3_GetValues(uint32_t Idx, uint32_t Type);
char*	Sensor_RS485_SoilMoistureLevel4_GetValues(uint32_t Idx, uint32_t Type);
char*	Sensor_RS485_SoilMoistureLevel5_GetValues(uint32_t Idx, uint32_t Type);
char*	Sensor_RS485_SoilMoistureLevel6_GetValues(uint32_t Idx, uint32_t Type);
char*	Sensor_RS485_SoilPhExp_GetValues(uint32_t Idx, uint32_t Type);


// Structs
struct Sensor_RS485_ConfigStruct
{
	char*		(*GetValues)(uint32_t Idx, uint32_t Type);
	uint8_t		SensorId;
	uint8_t		Function;
	uint8_t		DataLengthTx;
	uint8_t		DataLengthRx;
	uint8_t		DataTx[32];
	uint32_t	TimeInit;
	uint32_t	TimeRead;
	uint32_t 	Recurrence; // in hours (1, 2, 3, 4, 6, 12, 24)
};

// Const data struct
const struct Sensor_RS485_ConfigStruct Sensor_RS485_Config[] =
{
	///////////////////	Photosynthetically sensor //////////////
	{	Sensor_RS485_RDPAS01_GetValues,
		13,
		3,
		8,
		7,
		{0x00, 0x00, 0x00, 0x01},
		2000,
		1000,				// response time 1 second
		1
	},
	///////////////////	Illumination sensor ///////////////////
	{	Sensor_RS485_ISRDWO01_GetValues,
		16,
		3,
		8,
		7,
		{0x00, 0x00, 0x00, 0x01},
		2200,
		1000,				// response time 1 second
		1
	},
	///////////////////	Soil 8 in 1 sensor ///////////////////
	{	Sensor_RS485_RDS8IN1_GetValues,
		8,
		3,
		8,
		21,
		{0x00, 0x00, 0x00, 0x08},
		2400,
		15000,				// response time 15 seconds
		1
	},
	///////////////////	Soil CO2 sensor ///////////////////
	{	Sensor_RS485_SCO201_GetValues,
		3,
		3,
		8,
		7,
		{0x00, 0x00, 0x00, 0x01},
		2600,
		(3*60+90)*1000,		// heating 3 minutes, response time 90 seconds
		1
	},
	///////////////////	Soil O2 sensor ///////////////////
	{	Sensor_RS485_SOR01_GetValues,
		37,
		3,
		8,
		7,
		{0x00, 0x00, 0x00, 0x01},
		2800,
		15000,				// response time 15 seconds
		1
	},
	///////////////////	Soil CH4 sensor ///////////////////
	{	Sensor_RS485_SC2H4R01_GetValues,
		6,
		3,
		8,
		7,
		{0x00, 0x00, 0x00, 0x01},
		3000,
		(3*60+60)*1000,		// heating 3 minutes, response time 60 seconds
		1
	},
	///////////////////	Air C2H4 sensor ///////////////////
	{	Sensor_RS485_RDC2H401_GetValues,
		38,
		3,
		8,
		7,
		{0x00, 0x08, 0x00, 0x01},
		3200,
		15000, 				// response time 15 seconds
		1
	},
	///////////////////	Wind speed sensor ///////////////////
	{	Sensor_RS485_RDWSMASA_GetValues,
		33,
		3,
		8,
		7,
		{0x00, 0x00, 0x00, 0x01},
		3400,
		4000, 				// response time 2 seconds
		1
	},
	///////////////////	SoilPh ///////////////////
	{	Sensor_RS485_SoilPh_GetValues,
		1,
		3,
		8,
		7,
		{0x00, 0x00, 0x00, 0x01},
		3600,				// ATENTION SLOW SENSOR!!! Let delay of at least 400ms until next sensor!
		4000, 				// response time 4 seconds
		1
	},
	///////////////////	SoilTempHumSalEcNpk ///////////////////
	{	Sensor_RS485_SoilTempHumSalEcNpk_GetValues,
		129,
		3,
		8,
		19,
		{0x00, 0x00, 0x00, 0x07},
		4000,
		5000, 				// response time 5 seconds
		1
	},
	///////////////////	SoilMoistureLevel2 ///////////////////
	{	Sensor_RS485_SoilMoistureLevel2_GetValues,
		130,
		3,
		8,
		7,
		{0x00, 0x00, 0x00, 0x01},
		4200,
		2000, 				// response time 2 seconds
		1
	},
	///////////////////	SoilMoistureLevel3 ///////////////////
	{	Sensor_RS485_SoilMoistureLevel3_GetValues,
		131,
		3,
		8,
		7,
		{0x00, 0x00, 0x00, 0x01},
		4400,
		2000, 				// response time 2 seconds
		1
	},
	///////////////////	SoilMoistureLevel4 ///////////////////
	{	Sensor_RS485_SoilMoistureLevel4_GetValues,
		132,
		3,
		8,
		7,
		{0x00, 0x00, 0x00, 0x01},
		4600,
		2000, 				// response time 2 seconds
		1
	},
	///////////////////	SoilMoistureLevel5 ///////////////////
	{	Sensor_RS485_SoilMoistureLevel5_GetValues,
		133,
		3,
		8,
		7,
		{0x00, 0x00, 0x00, 0x01},
		4800,
		2000, 				// response time 2 seconds
		1
	},
	///////////////////	SoilMoistureLevel6 ///////////////////
	{	Sensor_RS485_SoilMoistureLevel6_GetValues,
		134,
		3,
		8,
		7,
		{0x00, 0x00, 0x00, 0x01},
		5000,
		2000, 				// response time 2 seconds
		1
	},
	///////////////////	SoilPhExp ///////////////////
	{	Sensor_RS485_SoilPhExp_GetValues,
		4,
		3,
		8,
		7,
		{0x00, 0x00, 0x00, 0x01},
		5200,
		15000, 				// response time 15 seconds
		1
	},
};

const uint8_t Sensor_RS485_ConfigNumber = sizeof(Sensor_RS485_Config)/sizeof(struct Sensor_RS485_ConfigStruct);

// Data
uint8_t Sensor_RS485_TxBuffer[64];
uint8_t Sensor_RS485_RxBuffer[64];
char	Sensor_RS485_ReturnValues[256];
char	Sensor_RS485_ReturnAllValues[2048];
char*	Sensor_RS485_ReturnAllValuesPtr;

uint8_t	 Sensor_RS485_State[64];
uint8_t	 Sensor_RS485_MeasurementDone[64];
uint32_t Sensor_RS485_MeasurementNumber;
uint32_t Sensor_RS485_TimeStamp;
uint32_t Sensor_RS485_BlockTimeStamp;
uint32_t Sensor_StartHour;


// Private operations
uint16_t Sensor_RS485_Crc16 (uint8_t* Data, uint32_t Length)
{
    static const uint16_t CRCTable[] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040};

    uint8_t  Tmp;
    uint16_t CRCWord = 0xFFFF;
    uint32_t I = Length;
    while (I--)
    {
        Tmp = (*Data++) ^ CRCWord;
        CRCWord >>= 8;
        CRCWord  ^= CRCTable[Tmp&0xFF];
    }
    return CRCWord;
}

HAL_StatusTypeDef Sensor_RS485_UsualComm(uint8_t Idx)
{
	Sensor_RS485_TxBuffer[0] = Sensor_RS485_Config[Idx].SensorId;
	Sensor_RS485_TxBuffer[1] = Sensor_RS485_Config[Idx].Function;
	uint32_t DataLengthTx = Sensor_RS485_Config[Idx].DataLengthTx;
	memcpy(&Sensor_RS485_TxBuffer[2], Sensor_RS485_Config[Idx].DataTx, DataLengthTx-4);
	uint16_t Crc16 = Sensor_RS485_Crc16(Sensor_RS485_TxBuffer, DataLengthTx-2);
	Sensor_RS485_TxBuffer[DataLengthTx-2] = (uint8_t)(Crc16);
	Sensor_RS485_TxBuffer[DataLengthTx-1] = (uint8_t)(Crc16>>8);
	HAL_UART_Abort(&huart2);
	HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart2, Sensor_RS485_TxBuffer, DataLengthTx, 50);
	HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_RESET);
	HAL_StatusTypeDef Status = HAL_UART_Receive(&huart2, Sensor_RS485_RxBuffer, Sensor_RS485_Config[Idx].DataLengthRx, 150);
	HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_SET);
	return Status;
}

HAL_StatusTypeDef Sensor_RS485_SetSensorAddress(void)
{
	//static const uint8_t SetAddrCmd[] = {1, 0x03, 0x00, 0x00, 0x00, 01};
	static const uint8_t SetAddrCmd[] = {0x01, 0x06, 0X07, 0xD0, 0x00, 0x08};
	HAL_Delay(1000);
	Sensor_RS485_TxBuffer[0] = SetAddrCmd[0]; Sensor_RS485_TxBuffer[1] = SetAddrCmd[1];
	Sensor_RS485_TxBuffer[2] = SetAddrCmd[2]; Sensor_RS485_TxBuffer[3] = SetAddrCmd[3];
	Sensor_RS485_TxBuffer[4] = SetAddrCmd[4]; Sensor_RS485_TxBuffer[5] = SetAddrCmd[5];
	uint16_t Crc16 = Sensor_RS485_Crc16(Sensor_RS485_TxBuffer, 6);
	Sensor_RS485_TxBuffer[6] = (uint8_t)(Crc16);
	Sensor_RS485_TxBuffer[7] = (uint8_t)(Crc16>>8);
	HAL_UART_Abort(&huart2);
	HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart2, Sensor_RS485_TxBuffer, 8, 50);
	HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_RESET);
	HAL_StatusTypeDef Status = HAL_UART_Receive(&huart2, Sensor_RS485_RxBuffer, 7, 150);
	HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_SET);
	while (1)
		(void)HAL_IWDG_Refresh(&hiwdg1);
	return Status;
}


char* Sensor_RS485_RDPAS01_GetValues(uint32_t Idx, uint32_t Type)
{
	char*	Dest = Sensor_RS485_ReturnValues;
	char	TmpTxt[16];
	double	Val;

	Dest = Utils_CopyString("Photosynthesis:", Dest);
	if (Type && (Sensor_RS485_UsualComm(Idx) == HAL_OK))
		if ((Sensor_RS485_RxBuffer[0] == Sensor_RS485_Config[Idx].SensorId) && (Sensor_RS485_RxBuffer[1] == Sensor_RS485_Config[Idx].Function))
		{
			// Return the value
			Val = (double)((uint32_t)Sensor_RS485_RxBuffer[3]*256 + Sensor_RS485_RxBuffer[4]);
			// convert umol/m2s to W/m2 (1W = 1Nm/s) => 1 μmole/m2/s ≈ 0.21739130434 W/m2
			*Dest++ = ' '; Dest = Utils_CopyString(itoa((uint32_t)(Val*0.21739130434F), TmpTxt, 10), Dest); Dest = Utils_CopyString("Nm/s/m2", Dest);
		}
	*Dest = '\0'; return Sensor_RS485_ReturnValues;
}

char* Sensor_RS485_ISRDWO01_GetValues(uint32_t Idx, uint32_t Type)
{
	char*	 Dest = Sensor_RS485_ReturnValues;
	char	 TmpTxt[16];
	uint32_t Val;

	Dest = Utils_CopyString("Illumination:", Dest);
	if (Type && (Sensor_RS485_UsualComm(Idx) == HAL_OK))
		if ((Sensor_RS485_RxBuffer[0] == Sensor_RS485_Config[Idx].SensorId) && (Sensor_RS485_RxBuffer[1] == Sensor_RS485_Config[Idx].Function))
		{
			// Return the value
			Val = (uint32_t)Sensor_RS485_RxBuffer[3]*256 + Sensor_RS485_RxBuffer[4];
			*Dest++ = ' '; Dest = Utils_CopyString(itoa(Val*10, TmpTxt, 10), Dest); Dest = Utils_CopyString("lux", Dest);
		}
	*Dest = '\0'; return Sensor_RS485_ReturnValues;
}

char* Sensor_RS485_RDS8IN1_GetValues(uint32_t Idx, uint32_t Type)
{
	char*	 Dest = Sensor_RS485_ReturnValues;
	char	 TmpTxt[16];
	uint32_t Val;
	int16_t  Val16;

	if (Type && (Sensor_RS485_UsualComm(Idx) == HAL_OK))
		if ((Sensor_RS485_RxBuffer[0] == Sensor_RS485_Config[Idx].SensorId) &&
			(Sensor_RS485_RxBuffer[1] == Sensor_RS485_Config[Idx].Function))
		{
			// Return the values
			Dest = Utils_CopyString("SoilMoisture: ", Dest);
			Val = (uint32_t)Sensor_RS485_RxBuffer[3]*256 + Sensor_RS485_RxBuffer[4];
			Dest = Utils_CopyString(itoa(Val/10, TmpTxt, 10), Dest);
			*Dest++ = '.'; *Dest++ = '0' + (Val%10);
			Dest = Utils_CopyString("%\n", Dest);

			Dest = Utils_CopyString("SoilTemperature: ", Dest);
			Val16 = (int16_t)((int32_t)Sensor_RS485_RxBuffer[5]*256 + Sensor_RS485_RxBuffer[6]);
			if (Val16 < 0)
				{ Val16 = -Val16; *Dest++ = '-';}
			Dest = Utils_CopyString(itoa(Val16/10, TmpTxt, 10), Dest);
			*Dest++ = '.'; *Dest++ = '0' + (Val16%10);
			Dest = Utils_CopyString("C\n", Dest);

			Dest = Utils_CopyString("SoilEC: ", Dest);
			Val = (uint32_t)Sensor_RS485_RxBuffer[7]*256 + Sensor_RS485_RxBuffer[8];
			Dest = Utils_CopyString(itoa(Val, TmpTxt, 10), Dest);
			Dest = Utils_CopyString("us/cm\n", Dest);

			Dest = Utils_CopyString("SoilPH: ", Dest);
			Val = (uint32_t)Sensor_RS485_RxBuffer[9]*256 + Sensor_RS485_RxBuffer[10];
			Dest = Utils_CopyString(itoa(Val/10, TmpTxt, 10), Dest);
			*Dest++ = '.'; *Dest++ = '0' + (Val%10); *Dest++ = '\n';

			Dest = Utils_CopyString("SoilN: ", Dest);
			Val = (uint32_t)Sensor_RS485_RxBuffer[11]*256 + Sensor_RS485_RxBuffer[12];
			Dest = Utils_CopyString(itoa(Val, TmpTxt, 10), Dest);
			Dest = Utils_CopyString("mg/kg\n", Dest);
			Dest = Utils_CopyString("SoilP: ", Dest);
			Val = (uint32_t)Sensor_RS485_RxBuffer[13]*256 + Sensor_RS485_RxBuffer[14];
			Dest = Utils_CopyString(itoa(Val, TmpTxt, 10), Dest);
			Dest = Utils_CopyString("mg/kg\n", Dest);
			Dest = Utils_CopyString("SoilK: ", Dest);
			Val = (uint32_t)Sensor_RS485_RxBuffer[15]*256 + Sensor_RS485_RxBuffer[16];
			Dest = Utils_CopyString(itoa(Val, TmpTxt, 10), Dest);
			Dest = Utils_CopyString("mg/kg\n", Dest);

			Dest = Utils_CopyString("SoilSalinity: ", Dest);
			Val = (uint32_t)Sensor_RS485_RxBuffer[17]*256 + Sensor_RS485_RxBuffer[18];
			Dest = Utils_CopyString(itoa(Val, TmpTxt, 10), Dest);
			Dest = Utils_CopyString("ppm", Dest);
		}
	if (Dest == Sensor_RS485_ReturnValues)
		Dest = Utils_CopyString("SoilMoisture:\nSoilTemperature:\nSoilEC:\nSoilPH:\nSoilN:\nSoilP:\nSoilK:\nSoilSalinity:", Dest);
	*Dest = '\0'; return Sensor_RS485_ReturnValues;
}

char* Sensor_RS485_SCO201_GetValues(uint32_t Idx, uint32_t Type)
{
	char*	 Dest = Sensor_RS485_ReturnValues;
	char	 TmpTxt[16];
	uint32_t Val;

	Dest = Utils_CopyString("SoilCO2:", Dest);
	if (Type && (Sensor_RS485_UsualComm(Idx) == HAL_OK))
		if ((Sensor_RS485_RxBuffer[0] == Sensor_RS485_Config[Idx].SensorId) &&
			(Sensor_RS485_RxBuffer[1] == Sensor_RS485_Config[Idx].Function))
		{
			// Return the value
			Val = (uint32_t)Sensor_RS485_RxBuffer[3]*256 + Sensor_RS485_RxBuffer[4];
			*Dest++ = ' '; Dest = Utils_CopyString(itoa(Val, TmpTxt, 10), Dest); Dest = Utils_CopyString("ppm", Dest);
		}
	*Dest = '\0'; return Sensor_RS485_ReturnValues;
}

char* Sensor_RS485_SOR01_GetValues(uint32_t Idx, uint32_t Type)
{
	char*	 Dest = Sensor_RS485_ReturnValues;
	char	 TmpTxt[16];
	uint32_t Val;

	Dest = Utils_CopyString("SoilO2:", Dest);
	if (Type && (Sensor_RS485_UsualComm(Idx) == HAL_OK))
		if ((Sensor_RS485_RxBuffer[0] == Sensor_RS485_Config[Idx].SensorId) &&
			(Sensor_RS485_RxBuffer[1] == Sensor_RS485_Config[Idx].Function))
		{
			// Return the value
			Val = (uint32_t)Sensor_RS485_RxBuffer[3]*256 + Sensor_RS485_RxBuffer[4];
			*Dest++ = ' '; Dest = Utils_CopyString(itoa(Val/10, TmpTxt, 10), Dest); *Dest++ = '.'; *Dest++ = '0' + (Val%10); *Dest++ = '%';
		}
	*Dest = '\0'; return Sensor_RS485_ReturnValues;
}

char* Sensor_RS485_SC2H4R01_GetValues(uint32_t Idx, uint32_t Type)
{
	char*	 Dest = Sensor_RS485_ReturnValues;
	char	 TmpTxt[16];
	uint32_t Val;

	Dest = Utils_CopyString("SoilC2H4:", Dest);
	if (Type && (Sensor_RS485_UsualComm(Idx) == HAL_OK))
		if ((Sensor_RS485_RxBuffer[0] == Sensor_RS485_Config[Idx].SensorId) &&
			(Sensor_RS485_RxBuffer[1] == Sensor_RS485_Config[Idx].Function))
		{
			// Return the value
			Val = (uint32_t)Sensor_RS485_RxBuffer[3]*256 + Sensor_RS485_RxBuffer[4];
			*Dest++ = ' '; Dest = Utils_CopyString(itoa(Val/10, TmpTxt, 10), Dest); *Dest++ = '.'; *Dest++ = '0' + (Val%10); Dest = Utils_CopyString("ppm", Dest);
		}
	*Dest = '\0'; return Sensor_RS485_ReturnValues;
}

char* Sensor_RS485_RDC2H401_GetValues(uint32_t Idx, uint32_t Type)
{
	char*	 Dest = Sensor_RS485_ReturnValues;
	char	 TmpTxt[16];
	uint32_t Val;

	Dest = Utils_CopyString("AirC2H4:", Dest);
	if (Type && (Sensor_RS485_UsualComm(Idx) == HAL_OK))
		if ((Sensor_RS485_RxBuffer[0] == Sensor_RS485_Config[Idx].SensorId) &&
			(Sensor_RS485_RxBuffer[1] == Sensor_RS485_Config[Idx].Function))
		{
			// Return the value
			Val = (uint32_t)Sensor_RS485_RxBuffer[3]*256 + Sensor_RS485_RxBuffer[4];
			*Dest++ = ' '; Dest = Utils_CopyString(itoa(Val/10, TmpTxt, 10), Dest); *Dest++ = '.'; *Dest++ = '0' + (Val%10); Dest = Utils_CopyString("ppm", Dest);
		}
	 *Dest = '\0'; return Sensor_RS485_ReturnValues;
}

char* Sensor_RS485_RDWSMASA_GetValues(uint32_t Idx, uint32_t Type)
{
	char*	 Dest = Sensor_RS485_ReturnValues;
	char	 TmpTxt[16];
	uint32_t Val;

	Dest = Utils_CopyString("WindSpeed:", Dest);
	if (Type && (Sensor_RS485_UsualComm(Idx) == HAL_OK))
		if ((Sensor_RS485_RxBuffer[0] == Sensor_RS485_Config[Idx].SensorId) &&
			(Sensor_RS485_RxBuffer[1] == Sensor_RS485_Config[Idx].Function))
		{
			// Return the value
			Val = (uint32_t)Sensor_RS485_RxBuffer[3]*256 + Sensor_RS485_RxBuffer[4];
			*Dest++ = ' '; Dest = Utils_CopyString(itoa(Val/100, TmpTxt, 10), Dest); *Dest++ = '.';
			Dest = Utils_CopyString(itoa(Val%100, TmpTxt, 10), Dest); Dest = Utils_CopyString("m/s", Dest);
		}
	 *Dest = '\0'; return Sensor_RS485_ReturnValues;
}

char* Sensor_RS485_SoilPh_GetValues(uint32_t Idx, uint32_t Type)
{
	char*	 Dest = Sensor_RS485_ReturnValues;
	char	 TmpTxt[16];
	uint32_t Val;

	Dest = Utils_CopyString("SoilPH:", Dest);
	if (Type && (Sensor_RS485_UsualComm(Idx) == HAL_OK))
		if ((Sensor_RS485_RxBuffer[0] == Sensor_RS485_Config[Idx].SensorId) &&
			(Sensor_RS485_RxBuffer[1] == Sensor_RS485_Config[Idx].Function))
		{
			// Return the value
			Val = (uint32_t)Sensor_RS485_RxBuffer[3]*256 + Sensor_RS485_RxBuffer[4];
			*Dest++ = ' '; Dest = Utils_CopyString(itoa(Val/100, TmpTxt, 10), Dest); *Dest++ = '.';
			Dest = Utils_CopyString(itoa(Val%100, TmpTxt, 10), Dest);
		}
	 *Dest = '\0'; return Sensor_RS485_ReturnValues;
}

char* Sensor_RS485_SoilPhExp_GetValues(uint32_t Idx, uint32_t Type)
{
	char*	 Dest = Sensor_RS485_ReturnValues;
	char	 TmpTxt[16];
	uint32_t Val;

	Dest = Utils_CopyString("SoilPH:", Dest);
	if (Type && (Sensor_RS485_UsualComm(Idx) == HAL_OK))
		if ((Sensor_RS485_RxBuffer[0] == Sensor_RS485_Config[Idx].SensorId) &&
			(Sensor_RS485_RxBuffer[1] == Sensor_RS485_Config[Idx].Function))
		{
			// Return the value
			Val = (uint32_t)Sensor_RS485_RxBuffer[3]*256 + Sensor_RS485_RxBuffer[4];
			*Dest++ = ' '; Dest = Utils_CopyString(itoa(Val/100, TmpTxt, 10), Dest); *Dest++ = '.';
			Dest = Utils_CopyString(itoa(Val%100, TmpTxt, 10), Dest);
		}
	 *Dest = '\0'; return Sensor_RS485_ReturnValues;
}


char* Sensor_RS485_SoilTempHumSalEcNpk_GetValues(uint32_t Idx, uint32_t Type)
{
	char*	 Dest = Sensor_RS485_ReturnValues;
	char	 TmpTxt[16];
	uint32_t Val;
	int16_t  Val16;

	if (Type && (Sensor_RS485_UsualComm(Idx) == HAL_OK))
		if ((Sensor_RS485_RxBuffer[0] == Sensor_RS485_Config[Idx].SensorId) &&
			(Sensor_RS485_RxBuffer[1] == Sensor_RS485_Config[Idx].Function))
		{
			// Return the values
			Dest = Utils_CopyString("SoilTemperature: ", Dest);
			Val16 = (int16_t)((int32_t)Sensor_RS485_RxBuffer[3]*256 + Sensor_RS485_RxBuffer[4]);
			if (Val16 < 0)
				{ Val16 = -Val16; *Dest++ = '-';}
			Dest = Utils_CopyString(itoa(Val16/10, TmpTxt, 10), Dest);
			*Dest++ = '.'; *Dest++ = '0' + (Val16%10);
			Dest = Utils_CopyString("C\n", Dest);

			Dest = Utils_CopyString("SoilMoisture: ", Dest);
			Val = (uint32_t)Sensor_RS485_RxBuffer[5]*256 + Sensor_RS485_RxBuffer[6];
			Dest = Utils_CopyString(itoa(Val/10, TmpTxt, 10), Dest);
			*Dest++ = '.'; *Dest++ = '0' + (Val%10);
			Dest = Utils_CopyString("%\n", Dest);

			Dest = Utils_CopyString("SoilSalinity: ", Dest);
			Val = (uint32_t)Sensor_RS485_RxBuffer[7]*256 + Sensor_RS485_RxBuffer[8];
			Dest = Utils_CopyString(itoa((Val*55)/100, TmpTxt, 10), Dest);
			Dest = Utils_CopyString("ppm\n", Dest);

			Dest = Utils_CopyString("SoilEC: ", Dest);
			Val = (uint32_t)Sensor_RS485_RxBuffer[9]*256 + Sensor_RS485_RxBuffer[10];
			Dest = Utils_CopyString(itoa(Val, TmpTxt, 10), Dest);
			Dest = Utils_CopyString("us/cm\n", Dest);

			Dest = Utils_CopyString("SoilN: ", Dest);
			Val = (uint32_t)Sensor_RS485_RxBuffer[11]*256 + Sensor_RS485_RxBuffer[12];
			Dest = Utils_CopyString(itoa(Val, TmpTxt, 10), Dest);
			Dest = Utils_CopyString("mg/kg\n", Dest);
			Dest = Utils_CopyString("SoilP: ", Dest);
			Val = (uint32_t)Sensor_RS485_RxBuffer[13]*256 + Sensor_RS485_RxBuffer[14];
			Dest = Utils_CopyString(itoa(Val, TmpTxt, 10), Dest);
			Dest = Utils_CopyString("mg/kg\n", Dest);
			Dest = Utils_CopyString("SoilK: ", Dest);
			Val = (uint32_t)Sensor_RS485_RxBuffer[15]*256 + Sensor_RS485_RxBuffer[16];
			Dest = Utils_CopyString(itoa(Val, TmpTxt, 10), Dest);
			Dest = Utils_CopyString("mg/kg", Dest);
		}
	if (Dest == Sensor_RS485_ReturnValues)
		Dest = Utils_CopyString("SoilTemperature:\nSoilMoisture:\nSoilSalinity:\nSoilEC:\nSoilPH:\nSoilN:\nSoilP:\nSoilK:", Dest);
	*Dest = '\0'; return Sensor_RS485_ReturnValues;
}

char* Sensor_RS485_SoilMoistureLevels_GetValues(uint32_t Idx, uint32_t Type, uint32_t Level)
{
	char*	 Dest = Sensor_RS485_ReturnValues;
	char	 TmpTxt[16];
	uint32_t Val;

	Dest = Utils_CopyString("SoilMoistureL", Dest); *Dest++ = '0' + Level; *Dest++ = ':';
	if (Type && (Sensor_RS485_UsualComm(Idx) == HAL_OK))
		if ((Sensor_RS485_RxBuffer[0] == Sensor_RS485_Config[Idx].SensorId) &&
			(Sensor_RS485_RxBuffer[1] == Sensor_RS485_Config[Idx].Function))
		{
			// Return the value
			Val = (uint32_t)Sensor_RS485_RxBuffer[3]*256 + Sensor_RS485_RxBuffer[4];
			*Dest++ = ' '; Dest = Utils_CopyString(itoa(Val/10, TmpTxt, 10), Dest); *Dest++ = '.';
			*Dest++ = '0' + (Val%10); *Dest++ = '%';
		}
	 *Dest = '\0'; return Sensor_RS485_ReturnValues;
}

char* Sensor_RS485_SoilMoistureLevel2_GetValues(uint32_t Idx, uint32_t Type)
{return Sensor_RS485_SoilMoistureLevels_GetValues(Idx, Type, 2);}
char* Sensor_RS485_SoilMoistureLevel3_GetValues(uint32_t Idx, uint32_t Type)
{return Sensor_RS485_SoilMoistureLevels_GetValues(Idx, Type, 3);}
char* Sensor_RS485_SoilMoistureLevel4_GetValues(uint32_t Idx, uint32_t Type)
{return Sensor_RS485_SoilMoistureLevels_GetValues(Idx, Type, 4);}
char* Sensor_RS485_SoilMoistureLevel5_GetValues(uint32_t Idx, uint32_t Type)
{return Sensor_RS485_SoilMoistureLevels_GetValues(Idx, Type, 5);}
char* Sensor_RS485_SoilMoistureLevel6_GetValues(uint32_t Idx, uint32_t Type)
{return Sensor_RS485_SoilMoistureLevels_GetValues(Idx, Type, 6);}


// Public operations
void Sensor_RS485_Init(void)
{
	HAL_GPIO_WritePin(EN_12V_GPIO_Port, EN_12V_Pin, GPIO_PIN_SET);				// Enable the 12V main supply
	HAL_Delay(250);
	HAL_GPIO_WritePin(EN_12V_SW_GPIO_Port, EN_12V_SW_Pin, GPIO_PIN_SET);		// Enable the 12V secondary supply
	DevStatus[DEVSTATUS_SENSORS_RS485_STATUS_IDX] = SENSOR_RS485_NOT_CHECKED;
	for (uint32_t I=0; I<Sensor_RS485_ConfigNumber; I++)
		Sensor_RS485_State[I] = SENSOR_RS485_STATE_INIT;
	Sensor_RS485_TimeStamp = HAL_GetTick();
//	Sensor_RS485_SetSensorAddress();		// Used to set address of the sensor
}

void Sensor_RS485_InitServer(void)
{
	uint32_t CheckCnt = 0;
	uint32_t PresentCnt = 0;
	uint32_t Idx, Idx2;

	if (Sensor_RS485_GetGlobalStatus() == SENSOR_RS485_NOT_CHECKED)
	{
		for (Idx=0; Idx<Sensor_RS485_ConfigNumber; Idx++)
		{
			if (Sensor_RS485_State[Idx] == SENSOR_RS485_STATE_INIT)
				if ((HAL_GetTick() - Sensor_RS485_TimeStamp) > Sensor_RS485_Config[Idx].TimeInit)
				{
					DevStatus[DEVSTATUS_SENSOR_RS485_STATUS_IDX+Idx] = SENSOR_RS485_NOTPRESENT;
					if (Sensor_RS485_UsualComm(Idx) == HAL_OK)
						if ((Sensor_RS485_RxBuffer[0] == Sensor_RS485_Config[Idx].SensorId) &&
							(Sensor_RS485_RxBuffer[1] == Sensor_RS485_Config[Idx].Function))
							DevStatus[DEVSTATUS_SENSOR_RS485_STATUS_IDX+Idx] = SENSOR_RS485_PRESENT;
					Sensor_RS485_State[Idx] = SENSOR_RS485_STATE_IDLE;
				}

			if (DevStatus[DEVSTATUS_SENSOR_RS485_STATUS_IDX+Idx] == SENSOR_RS485_PRESENT)
				PresentCnt++;
			if (DevStatus[DEVSTATUS_SENSOR_RS485_STATUS_IDX+Idx] != SENSOR_RS485_NOT_CHECKED)
				CheckCnt++;
		}
		// All sensors checked?
		if (CheckCnt == Sensor_RS485_ConfigNumber)
		{
			DevStatus[DEVSTATUS_SENSORS_RS485_STATUS_IDX] = (PresentCnt != 0) ? SENSOR_RS485_PRESENT:SENSOR_RS485_NOTPRESENT;
			// Check sensors on 12V_SW (on 12V power switch)
			HAL_GPIO_WritePin(EN_12V_SW_GPIO_Port, EN_12V_SW_Pin, GPIO_PIN_RESET);	// Disable the 12V SW supply
			for (Idx2=0; Idx2<Sensor_RS485_ConfigNumber; Idx2++)
				if (DevStatus[DEVSTATUS_SENSOR_RS485_STATUS_IDX+Idx2] == SENSOR_RS485_PRESENT)
					if (Sensor_RS485_UsualComm(Idx2) != HAL_OK)
						DevStatus[DEVSTATUS_SENSOR_RS485_STATUS_IDX+Idx2] = SENSOR_RS485_PRESENT_SW;
			HAL_GPIO_WritePin(EN_12V_GPIO_Port, EN_12V_Pin, GPIO_PIN_RESET);	// Disable the 12V main supply
		}
	}
}


void Sensor_RS485_StartMeasurement(void)
{
	Sensor_RS485_MeasurementNumber = 0;
	if (Sensor_RS485_GetGlobalStatus() == SENSOR_RS485_PRESENT)
	{
		HAL_GPIO_WritePin(EN_3V3S_GPIO_Port, EN_3V3S_Pin, GPIO_PIN_SET);	// Enable the 3V3S power supply
		HAL_GPIO_WritePin(EN_12V_GPIO_Port, EN_12V_Pin, GPIO_PIN_SET);		// Enable the 12V main supply
		HAL_Delay(250);
		Sensor_StartHour = Rtc_GetCurrentHour();
		for (uint32_t Idx = 0; Idx<Sensor_RS485_ConfigNumber; Idx++)
			if (Sensor_RS485_GetStatus(Idx) >= SENSOR_RS485_PRESENT)
				if ((Sensor_StartHour % Sensor_RS485_Config[Idx].Recurrence) == 0)
				{
					Sensor_RS485_MeasurementNumber++;
					Sensor_RS485_MeasurementDone[Idx] = 0;
					if (Sensor_RS485_GetStatus(Idx) == SENSOR_RS485_PRESENT_SW)
						HAL_GPIO_WritePin(EN_12V_SW_GPIO_Port, EN_12V_SW_Pin, GPIO_PIN_SET);		// Enable the 12V SW supply
				}
	}
	HAL_Delay(250);
	Sensor_RS485_ReturnAllValuesPtr = Sensor_RS485_ReturnAllValues;
	Sensor_RS485_TimeStamp 			= HAL_GetTick();
	Sensor_RS485_BlockTimeStamp 	= Sensor_RS485_TimeStamp;

}


uint32_t Sensor_RS485_GetMeasurementStatus(void)
{
	char* Dest = Sensor_RS485_ReturnAllValuesPtr;
	if ((HAL_GetTick() - Sensor_RS485_BlockTimeStamp) > 100)
	{
		for (uint32_t Idx=0; Idx<Sensor_RS485_ConfigNumber; Idx++)
			if (Sensor_RS485_GetStatus(Idx) >= SENSOR_RS485_PRESENT)
				if ((Sensor_StartHour % Sensor_RS485_Config[Idx].Recurrence) == 0)
					if (Sensor_RS485_MeasurementDone[Idx] == 0)
						if ((HAL_GetTick() - Sensor_RS485_TimeStamp) > Sensor_RS485_Config[Idx].TimeRead)
						{
							Sensor_RS485_MeasurementDone[Idx] = 1;
							Sensor_RS485_MeasurementNumber--;
							Dest = Utils_CopyString(Sensor_RS485_Config[Idx].GetValues(Idx, 1), Dest); *Dest++ = '\n';
							Sensor_RS485_BlockTimeStamp = HAL_GetTick();
							Sensor_RS485_ReturnAllValuesPtr = Dest;
							break;
						}
		if (Sensor_RS485_MeasurementNumber == 0)
		{
			for (uint32_t Idx=0; Idx<Sensor_RS485_ConfigNumber; Idx++)
				if (Sensor_RS485_GetStatus(Idx) >= SENSOR_RS485_PRESENT)
					if (Sensor_RS485_MeasurementDone[Idx] == 0)
						{Dest = Utils_CopyString(Sensor_RS485_Config[Idx].GetValues(Idx, 0), Dest); *Dest++ = '\n';}
			if (Dest != Sensor_RS485_ReturnAllValues)	Dest--;
			*Dest = '\0';
			return 1;
		}
	}
	return 0;
}


char* Sensor_RS485_GetValues(void)
{
	if (Sensor_RS485_ReturnAllValues[0] != '\0')
		return Sensor_RS485_ReturnAllValues;
	return NULL;
}


int32_t	Sensor_RS485_GetStatus(uint8_t Idx)
{
	return DevStatus[DEVSTATUS_SENSOR_RS485_STATUS_IDX+Idx];
}

int32_t	Sensor_RS485_GetGlobalStatus(void)
{
	return DevStatus[DEVSTATUS_SENSORS_RS485_STATUS_IDX];
}
