//	==========================================================================
//	calibrations.h
//	(c) 2020, Aurel Dumitru
//
//	Description:
//  Calibrations
//	==========================================================================

#ifndef CALIBRATIONS_H
#define CALIBRATIONS_H

// Includes
#include "stm32h7xx_hal.h"
#include "main.h"
#include "proc.h"

// Non Calibrations */
#define NC_INVALID_INTEGER					0x80000000

#define NC_MAX_FIRMWARE_SIZE                (320*1024) 	// minimum 256*1024
#define NC_UART_RX_CIRCULAR_BUFFER_LEN		(  8*1024)

#define NC_HEAP_SIZE						(NC_MAX_FIRMWARE_SIZE + NC_UART_RX_CIRCULAR_BUFFER_LEN)

#define NC_MODEM_POWERKEY_ON_TO_READY_TIME  20000

#define NC_IR_DETECTION_THRESHOLD		    0x400   // 4
#define NC_IR_DETECTION_LIMIT		    	0xBB00  // 187
#define NC_DETECTION_RECURRENCE             200     // in milliseconds

#define NC_TIME_TO_MICROSD_POWER_OFF        500	//in milliseconds

#define NC_GPS_WAIT_TIME					120000
#define NC_GPS_TIME_WAIT_TIME				300000

#define NC_MODEM_BAUDRATE					3000000

#define NC_MAX_VALID_CONTOURS				64
#define NC_MAX_ANALYSED_CONTOURS			3
#define NC_CONTOUR_MIN_PERIMETER            80
#define NC_CONTOUR_RESIDUAL_MAX_PERIMETER   300

#define NC_MIN_TEMPERATURE_DETECTION		40		//4.0 Celsius

#define NC_MAX_CONTOUR_IMAGE_PERCENTAGE		85

#define NC_RAIN_DROP_WINDOW_Y				480
#define NC_RAIN_DROP_WINDOW_X				480
#define NC_THRESHOLD_RAIN_DROP_COUNTER		 16

#define NC_BACKGROUND_CONTOUR_TEXT_COLOR	0x000000  // mandatory R = G = B
#define NC_CONTOUR_TEXT_COLOR				0xD0D0D0
#define NC_CONTOUR1_RGB_COLOR				0xFF4040
#define NC_CONTOUR2_RGB_COLOR				0x4040FF
#define NC_CONTOUR3_RGB_COLOR				0xFF40FF

#define NC_PIXEL_IN_MM						0.0666F  //0.06262F
#define NC_IR_BASE_VALUE					37

#define NC_MOTOR_TEST_HOUR					13
#define NC_MOTOR_OPENLOAD_TH				0x1000
#define NC_MOTOR_JAMMED_PERCENTAGE			98

#define NC_NUMBER_OF_FILES_TO_DOWNLOAD		2


struct C_ImsiToApnStruct
{
	char Imsi[8];
	char Apn[24];
};


// Calibrations
extern const char		C_HttpAddr[];
extern const char		C_FtpAddr[];
extern const char		C_FirmwareId[];

extern uint32_t			C_CameraHorizontalPixels;
extern uint32_t			C_CameraVerticalPixels;

extern const uint32_t	C_ModemBauds[];

extern const struct C_ImsiToApnStruct C_ImsiToApn[];
extern const uint32_t	C_ImsiToApnNumber;

extern const char		C_SmsPhoneNumbers[][12];
extern const uint8_t	C_SmsNumberOfPhoneNumbers;

extern const int8_t		C_IrDetectionThresholdOffset[];

// Insects
extern const uint16_t	C_NumberOfInsectsModels;
extern const struct Proc_InsectModelStruct C_InsectsModels[];

// Set value on 1 for debugging the option
extern const uint8_t	C_DeactivateMotors;
extern const uint8_t	C_DeactivateFirmwareCheck;
extern const uint8_t	C_ActivateContourDraw;
extern const uint8_t	C_DeactivateAutoGainExposure;
extern const uint8_t	C_ActivateBlueLedOnRain;
extern const uint8_t	C_ActivateNormMaskOverImage;

#endif
