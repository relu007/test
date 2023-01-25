/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

#include "st25dv_conf.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern ADC_HandleTypeDef hadc2;

extern DCMI_HandleTypeDef hdcmi;
extern DMA_HandleTypeDef hdma_dcmi_pssi;

extern I2C_HandleTypeDef hi2c2;
extern I2C_HandleTypeDef hi2c4;

extern IWDG_HandleTypeDef hiwdg1;

extern JPEG_HandleTypeDef hjpeg;

extern OSPI_HandleTypeDef hospi1;

extern RTC_HandleTypeDef hrtc;

extern SD_HandleTypeDef hsd2;

extern SPI_HandleTypeDef hspi6;
extern DMA_HandleTypeDef hdma_spi6_tx;

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim12;

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart10;
extern UART_HandleTypeDef huart7;
extern DMA_HandleTypeDef hdma_uart7_rx;
extern DMA_HandleTypeDef hdma_uart7_tx;

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RS232_RX_Pin GPIO_PIN_2
#define RS232_RX_GPIO_Port GPIOE
#define RS232_TX_Pin GPIO_PIN_3
#define RS232_TX_GPIO_Port GPIOE
#define EN_12V_SW_Pin GPIO_PIN_4
#define EN_12V_SW_GPIO_Port GPIOE
#define CAMERA_D6_Pin GPIO_PIN_5
#define CAMERA_D6_GPIO_Port GPIOE
#define CAMERA_D7_Pin GPIO_PIN_6
#define CAMERA_D7_GPIO_Port GPIOE
#define MODEM_TXD_Pin GPIO_PIN_6
#define MODEM_TXD_GPIO_Port GPIOF
#define MODEM_RXD_Pin GPIO_PIN_7
#define MODEM_RXD_GPIO_Port GPIOF
#define MODEM_RESET_Pin GPIO_PIN_8
#define MODEM_RESET_GPIO_Port GPIOF
#define MODEM_RTS_Pin GPIO_PIN_9
#define MODEM_RTS_GPIO_Port GPIOF
#define EN_3V3_Pin GPIO_PIN_10
#define EN_3V3_GPIO_Port GPIOF
#define EN_6V_Pin GPIO_PIN_0
#define EN_6V_GPIO_Port GPIOH
#define RS485_DE_Pin GPIO_PIN_1
#define RS485_DE_GPIO_Port GPIOH
#define TILT_Pin GPIO_PIN_0
#define TILT_GPIO_Port GPIOC
#define EN_LED_IR_Pin GPIO_PIN_1
#define EN_LED_IR_GPIO_Port GPIOC
#define ADC_PD_IR_Pin GPIO_PIN_2
#define ADC_PD_IR_GPIO_Port GPIOC
#define ADC_BAT_Pin GPIO_PIN_3
#define ADC_BAT_GPIO_Port GPIOC
#define LED_PWM_Pin GPIO_PIN_0
#define LED_PWM_GPIO_Port GPIOA
#define RS485_TX_Pin GPIO_PIN_2
#define RS485_TX_GPIO_Port GPIOA
#define RS485_RX_Pin GPIO_PIN_3
#define RS485_RX_GPIO_Port GPIOA
#define CAMERA_HSYNC_Pin GPIO_PIN_4
#define CAMERA_HSYNC_GPIO_Port GPIOA
#define RS485_RE_Pin GPIO_PIN_5
#define RS485_RE_GPIO_Port GPIOA
#define CAMERA_PIX_CLK_Pin GPIO_PIN_6
#define CAMERA_PIX_CLK_GPIO_Port GPIOA
#define ADC_SENSOR_Pin GPIO_PIN_4
#define ADC_SENSOR_GPIO_Port GPIOC
#define ADC_I_MOTOR_Pin GPIO_PIN_14
#define ADC_I_MOTOR_GPIO_Port GPIOF
#define NFC_LDO_DUMMY_Pin GPIO_PIN_12
#define NFC_LDO_DUMMY_GPIO_Port GPIOE
#define OCTOSPIM_RST_Pin GPIO_PIN_13
#define OCTOSPIM_RST_GPIO_Port GPIOE
#define NFC_INT_Pin GPIO_PIN_14
#define NFC_INT_GPIO_Port GPIOE
#define NFC_INT_EXTI_IRQn EXTI15_10_IRQn
#define VCC_NFC_Pin GPIO_PIN_15
#define VCC_NFC_GPIO_Port GPIOE
#define I2C_SCL_Pin GPIO_PIN_10
#define I2C_SCL_GPIO_Port GPIOB
#define I2C_SDA_Pin GPIO_PIN_11
#define I2C_SDA_GPIO_Port GPIOB
#define MOTOR2_SLP_Pin GPIO_PIN_13
#define MOTOR2_SLP_GPIO_Port GPIOB
#define MOTOR_EN_Pin GPIO_PIN_14
#define MOTOR_EN_GPIO_Port GPIOB
#define MOTOR2_PH_Pin GPIO_PIN_15
#define MOTOR2_PH_GPIO_Port GPIOB
#define MOTOR1_SLP_Pin GPIO_PIN_8
#define MOTOR1_SLP_GPIO_Port GPIOD
#define MOTOR1_PH_Pin GPIO_PIN_9
#define MOTOR1_PH_GPIO_Port GPIOD
#define FLASH_Pin GPIO_PIN_10
#define FLASH_GPIO_Port GPIOD
#define CAMERA_I2C_SCL_Pin GPIO_PIN_12
#define CAMERA_I2C_SCL_GPIO_Port GPIOD
#define CAMERA_I2C_SDA_Pin GPIO_PIN_13
#define CAMERA_I2C_SDA_GPIO_Port GPIOD
#define CAMERA_TRIGGER_Pin GPIO_PIN_6
#define CAMERA_TRIGGER_GPIO_Port GPIOG
#define CAMERA_RESET_Pin GPIO_PIN_7
#define CAMERA_RESET_GPIO_Port GPIOG
#define CAMERA_STBY_Pin GPIO_PIN_8
#define CAMERA_STBY_GPIO_Port GPIOG
#define CAMERA_D0_Pin GPIO_PIN_6
#define CAMERA_D0_GPIO_Port GPIOC
#define CAMERA_D1_Pin GPIO_PIN_7
#define CAMERA_D1_GPIO_Port GPIOC
#define CAMERA_D2_Pin GPIO_PIN_8
#define CAMERA_D2_GPIO_Port GPIOC
#define CAMERA_D3_Pin GPIO_PIN_9
#define CAMERA_D3_GPIO_Port GPIOC
#define CAMERA_EXTCLK_Pin GPIO_PIN_8
#define CAMERA_EXTCLK_GPIO_Port GPIOA
#define USB_DN_Pin GPIO_PIN_11
#define USB_DN_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define CAMERA_D8_Pin GPIO_PIN_10
#define CAMERA_D8_GPIO_Port GPIOC
#define CAMERA_D4_Pin GPIO_PIN_11
#define CAMERA_D4_GPIO_Port GPIOC
#define CAMERA_D9_Pin GPIO_PIN_12
#define CAMERA_D9_GPIO_Port GPIOC
#define EN_3V3S_Pin GPIO_PIN_0
#define EN_3V3S_GPIO_Port GPIOD
#define CAMERA_D11_Pin GPIO_PIN_2
#define CAMERA_D11_GPIO_Port GPIOD
#define CAMERA_D5_Pin GPIO_PIN_3
#define CAMERA_D5_GPIO_Port GPIOD
#define SD_CLK_Pin GPIO_PIN_6
#define SD_CLK_GPIO_Port GPIOD
#define SD_CMD_Pin GPIO_PIN_7
#define SD_CMD_GPIO_Port GPIOD
#define SD_D0_Pin GPIO_PIN_9
#define SD_D0_GPIO_Port GPIOG
#define SD_D1_Pin GPIO_PIN_10
#define SD_D1_GPIO_Port GPIOG
#define SD_D2_Pin GPIO_PIN_11
#define SD_D2_GPIO_Port GPIOG
#define SD_D3_Pin GPIO_PIN_12
#define SD_D3_GPIO_Port GPIOG
#define RGB_SCK_Pin GPIO_PIN_13
#define RGB_SCK_GPIO_Port GPIOG
#define RGB_SDI_Pin GPIO_PIN_14
#define RGB_SDI_GPIO_Port GPIOG
#define EN_12V_Pin GPIO_PIN_4
#define EN_12V_GPIO_Port GPIOB
#define CAMERA_D10_Pin GPIO_PIN_5
#define CAMERA_D10_GPIO_Port GPIOB
#define CAMERA_VSYNC_Pin GPIO_PIN_7
#define CAMERA_VSYNC_GPIO_Port GPIOB
#define EN_3V3AUX_Pin GPIO_PIN_0
#define EN_3V3AUX_GPIO_Port GPIOE
#define EN_BUZ_Pin GPIO_PIN_1
#define EN_BUZ_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
