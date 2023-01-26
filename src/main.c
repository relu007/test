/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
#include "calibrations.h"
#include "config.h"
#include "camera.h"
#include "sensor_shtc3.h"
#include "sensor_nfc.h"
#include "sensors.h"
#include "buzzer.h"
#include "custom_nfc04a1_nfctag.h"
#include "rgbled.h"
#include "rtc.h"
#include "file.h"
#include "utils.h"
#include "modem.h"
#include "motor_test.h"
#include "flap.h"
#include "usb.h"
#include "detect_lowpower.h"
#include "control.h"
#include "flash.h"
#include "jpeg_utils.h"
#include "devstatus.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 ADC_HandleTypeDef hadc2;

CRC_HandleTypeDef hcrc;

DCMI_HandleTypeDef hdcmi;
DMA_HandleTypeDef hdma_dcmi_pssi;

I2C_HandleTypeDef hi2c4;

IWDG_HandleTypeDef hiwdg1;

JPEG_HandleTypeDef hjpeg;

OSPI_HandleTypeDef hospi1;

RTC_HandleTypeDef hrtc;

SD_HandleTypeDef hsd2;

SPI_HandleTypeDef hspi6;

TIM_HandleTypeDef htim5;

UART_HandleTypeDef huart7;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart10;
DMA_HandleTypeDef hdma_uart7_rx;
DMA_HandleTypeDef hdma_uart7_tx;

MDMA_HandleTypeDef hmdma_mdma_channel40_sw_0;
MDMA_HandleTypeDef hmdma_mdma_channel41_sw_0;
MDMA_HandleTypeDef hmdma_mdma_channel42_sw_0;
MDMA_HandleTypeDef hmdma_mdma_channel43_sw_0;
/* USER CODE BEGIN PV */
RCC_OscInitTypeDef Main_RccOscInitStruct;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_MDMA_Init(void);
static void MX_DCMI_Init(void);
static void MX_OCTOSPI1_Init(void);
static void MX_SDMMC2_SD_Init(void);
static void MX_SPI6_Init(void);
static void MX_I2C4_Init(void);
static void MX_UART7_Init(void);
static void MX_ADC2_Init(void);
static void MX_JPEG_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM5_Init(void);
static void MX_CRC_Init(void);
static void MX_IWDG1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART10_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  DevStatus_Init();
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  DevStatus[DEVSTATUS_OSC32_STATUS_IDX] = DEVSTATUS_OSC32_OK;
  DevStatus_SetInitPhase(DEVSTATUS_OSC32_INIT_VALUE);

  /* Check if external oscillator is present */
  HAL_RCC_GetOscConfig(&Main_RccOscInitStruct);
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == 0)
  {
	  PWR->CR1 |= PWR_CR1_DBP;
	  while((PWR->CR1 & PWR_CR1_DBP) == 0U);
	  Main_RccOscInitStruct.LSEState = RCC_LSE_BYPASS;
	  __HAL_RCC_LSE_CONFIG(Main_RccOscInitStruct.LSEState);
	  uint32_t Timestamp = HAL_GetTick();
	  while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == 0)
		  if((HAL_GetTick() - Timestamp ) > 700) {Main_RccOscInitStruct.LSEState = RCC_LSE_ON; break;}
	  __HAL_RCC_LSE_CONFIG(RCC_LSE_OFF);
	  while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) != 0);
  }
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */
  DevStatus_SetInitPhase(DEVSTATUS_UNKNOWN_INIT_VALUE);
  __HAL_DBGMCU_FREEZE_IWDG1();
  __HAL_DBGMCU_FREEZE_TIM5();
  HAL_EnableDBGStopMode();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_MDMA_Init();
  MX_DCMI_Init();
  MX_OCTOSPI1_Init();
  MX_SDMMC2_SD_Init();
  MX_SPI6_Init();
  MX_I2C4_Init();
  MX_UART7_Init();
  MX_ADC2_Init();
  MX_JPEG_Init();
  MX_RTC_Init();
  MX_FATFS_Init();
  MX_USB_DEVICE_Init();
  MX_TIM5_Init();
  MX_CRC_Init();
  MX_IWDG1_Init();
  MX_USART2_UART_Init();
  MX_USART10_UART_Init();
  /* USER CODE BEGIN 2 */
  /* Load functions into ITCM RAM */
  extern unsigned char itcm_code_flash;
  extern unsigned char fast_code_start;
  extern unsigned char fast_code_end;
  memcpy(&fast_code_start, &itcm_code_flash, (int) (&fast_code_end - &fast_code_start));
    // Set the state of special pins in LOW power
  HAL_GPIO_WritePin(CAMERA_EXTCLK_GPIO_Port, CAMERA_EXTCLK_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(MODEM_RXD_GPIO_Port, MODEM_RXD_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RGB_SCK_GPIO_Port, RGB_SCK_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RGB_SDI_GPIO_Port, RGB_SDI_Pin, GPIO_PIN_RESET);
  // Start 1us timebase
  HAL_TIM_Base_Start(&htim5);
  // Start ADC calibration
  HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);
  // Set voltage scaling for low power stop mode to SVOS4
  HAL_PWREx_ControlStopModeVoltageScaling(PWR_REGULATOR_SVOS_SCALE4);
  // Enable FLASH low power mode in Dstop2
  HAL_PWREx_EnableFlashPowerDown();

  // Power cycle microSD card (wait 10x of discharge time of 20uF capacitors)
  HAL_Delay(500);
  // Set EN_3V3AUX to active level (HIGH)
  HAL_GPIO_WritePin(EN_3V3AUX_GPIO_Port, EN_3V3AUX_Pin, GPIO_PIN_SET);
  // Set EN_6V to active level (HIGH)
  HAL_GPIO_WritePin(EN_6V_GPIO_Port, EN_6V_Pin, GPIO_PIN_SET);
  // Set VCC_NFC to active level (HIGH)
  HAL_GPIO_WritePin(VCC_NFC_GPIO_Port, VCC_NFC_Pin, GPIO_PIN_SET);

  Camera_Init();
  DevStatus_SetDeviceType(0);	// check and set device type (insect or sensors; check if camera and hyperram are available)
  CUSTOM_NFCTAG_Init(0);
  Sensor_Nfc_Init();
  Rgbled_Init();
  Utils_InitMem();
  Flash_Init();
  File_Init();
  Modem_Init();
  Sensor_Shtc3_Init();
  JPEG_InitColorTables();

  Usb_InitialCheckTest();

  MX_USB_DEVICE_DeInit();
  Camera_EnterStandby();
  Config_ParseConfigFile();
  if (DevStatus_IsDeviceTypeInsectTrap())
	  Detect_Init();
  else
  {
	  HAL_DCMI_MspDeInit(&hdcmi);		// deinit DCMI
	  HAL_I2C_MspDeInit(&hi2c4);		// deinit camera I2C4 for camera communication
	  HAL_JPEG_MspDeInit(&hjpeg);		// deinit JPEG HW module
	  HAL_OSPI_MspDeInit(&hospi1);		// deinit OCTOSPI
  }
  Sensors_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // Enter low power
	  Detect_LowPower_Enter();
	  // Cycle Low power
	  Detect_LowPower_Server();
	  // Exit low power
	  Detect_LowPower_Exit();
	  // Something to do
	  Control_Start();
	  while (1)
	  {
		  Modem_Server();
		  Control_Server();
		  (void)HAL_IWDG_Refresh(&hiwdg1);
		  if (Control_IsIdle())
			  break;
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_CRSInitTypeDef RCC_CRSInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_HIGH);

  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSI);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSI
                              |RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = Main_RccOscInitStruct.LSEState;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 32;
  RCC_OscInitStruct.PLL.PLLN = 280;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI, RCC_MCODIV_4);

  /** Enable the SYSCFG APB clock
  */
  __HAL_RCC_CRS_CLK_ENABLE();

  /** Configures CRS
  */
  RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;
  RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_USB2;
  RCC_CRSInitStruct.Polarity = RCC_CRS_SYNC_POLARITY_RISING;
  RCC_CRSInitStruct.ReloadValue = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000,1000);
  RCC_CRSInitStruct.ErrorLimitValue = 34;
  RCC_CRSInitStruct.HSI48CalibrationValue = 32;

  HAL_RCCEx_CRSConfig(&RCC_CRSInitStruct);
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_OSPI|RCC_PERIPHCLK_SDMMC
                              |RCC_PERIPHCLK_UART7|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_CKPER;
  PeriphClkInitStruct.PLL2.PLL2M = 32;
  PeriphClkInitStruct.PLL2.PLL2N = 200;
  PeriphClkInitStruct.PLL2.PLL2P = 4;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_1;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.OspiClockSelection = RCC_OSPICLKSOURCE_PLL2;
  PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL2;
  PeriphClkInitStruct.CkperClockSelection = RCC_CLKPSOURCE_HSI;
  PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV4;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc2.Init.LowPowerAutoWait = DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  hadc2.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc2.Init.LeftBitShift = ADC_LEFTBITSHIFT_4;
  hadc2.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief DCMI Initialization Function
  * @param None
  * @retval None
  */
static void MX_DCMI_Init(void)
{

  /* USER CODE BEGIN DCMI_Init 0 */

  /* USER CODE END DCMI_Init 0 */

  /* USER CODE BEGIN DCMI_Init 1 */

  /* USER CODE END DCMI_Init 1 */
  hdcmi.Instance = DCMI;
  hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
  hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;
  hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_LOW;
  hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_LOW;
  hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
  hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_12B;
  hdcmi.Init.JPEGMode = DCMI_JPEG_DISABLE;
  hdcmi.Init.ByteSelectMode = DCMI_BSM_ALL;
  hdcmi.Init.ByteSelectStart = DCMI_OEBS_ODD;
  hdcmi.Init.LineSelectMode = DCMI_LSM_ALL;
  hdcmi.Init.LineSelectStart = DCMI_OELS_ODD;
  if (HAL_DCMI_Init(&hdcmi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DCMI_Init 2 */

  /* USER CODE END DCMI_Init 2 */

}

/**
  * @brief I2C4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C4_Init(void)
{

  /* USER CODE BEGIN I2C4_Init 0 */

  /* USER CODE END I2C4_Init 0 */

  /* USER CODE BEGIN I2C4_Init 1 */

  /* USER CODE END I2C4_Init 1 */
  hi2c4.Instance = I2C4;
  hi2c4.Init.Timing = 0x0060247E;
  hi2c4.Init.OwnAddress1 = 0;
  hi2c4.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c4.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c4.Init.OwnAddress2 = 0;
  hi2c4.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c4.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c4.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c4) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c4, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c4, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C4_Init 2 */

  /* USER CODE END I2C4_Init 2 */

}

/**
  * @brief IWDG1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG1_Init(void)
{

  /* USER CODE BEGIN IWDG1_Init 0 */

  /* USER CODE END IWDG1_Init 0 */

  /* USER CODE BEGIN IWDG1_Init 1 */

  /* USER CODE END IWDG1_Init 1 */
  hiwdg1.Instance = IWDG1;
  hiwdg1.Init.Prescaler = IWDG_PRESCALER_64;
  hiwdg1.Init.Window = 4095;
  hiwdg1.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG1_Init 2 */

  /* USER CODE END IWDG1_Init 2 */

}

/**
  * @brief JPEG Initialization Function
  * @param None
  * @retval None
  */
static void MX_JPEG_Init(void)
{

  /* USER CODE BEGIN JPEG_Init 0 */

  /* USER CODE END JPEG_Init 0 */

  /* USER CODE BEGIN JPEG_Init 1 */

  /* USER CODE END JPEG_Init 1 */
  hjpeg.Instance = JPEG;
  if (HAL_JPEG_Init(&hjpeg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN JPEG_Init 2 */

  /* USER CODE END JPEG_Init 2 */

}

/**
  * @brief OCTOSPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_OCTOSPI1_Init(void)
{

  /* USER CODE BEGIN OCTOSPI1_Init 0 */
  OSPI_HyperbusCmdTypeDef sCommand = {0};
  OSPI_MemoryMappedTypeDef sMemMappedCfg ={0};
  /* USER CODE END OCTOSPI1_Init 0 */

  OSPIM_CfgTypeDef sOspiManagerCfg = {0};
  OSPI_HyperbusCfgTypeDef sHyperBusCfg = {0};

  /* USER CODE BEGIN OCTOSPI1_Init 1 */
  DevStatus[DEVSTATUS_HYPERRAM_STATUS_IDX] = DEVSTATUS_HYPERRAM_OK;
  DevStatus_SetInitPhase(DEVSTATUS_HYPERRAM_INIT_VALUE);
  HAL_GPIO_WritePin(OCTOSPIM_RST_GPIO_Port, OCTOSPIM_RST_Pin, GPIO_PIN_RESET);
  HAL_Delay(2);
  HAL_GPIO_WritePin(OCTOSPIM_RST_GPIO_Port, OCTOSPIM_RST_Pin, GPIO_PIN_SET);
  HAL_Delay(2);
  /* USER CODE END OCTOSPI1_Init 1 */
  /* OCTOSPI1 parameter configuration*/
  hospi1.Instance = OCTOSPI1;
  hospi1.Init.FifoThreshold = 1;
  hospi1.Init.DualQuad = HAL_OSPI_DUALQUAD_DISABLE;
  hospi1.Init.MemoryType = HAL_OSPI_MEMTYPE_HYPERBUS;
  hospi1.Init.DeviceSize = 23;
  hospi1.Init.ChipSelectHighTime = 1;
  hospi1.Init.FreeRunningClock = HAL_OSPI_FREERUNCLK_DISABLE;
  hospi1.Init.ClockMode = HAL_OSPI_CLOCK_MODE_0;
  hospi1.Init.WrapSize = HAL_OSPI_WRAP_NOT_SUPPORTED;
  hospi1.Init.ClockPrescaler = 2;
  hospi1.Init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_NONE;
  hospi1.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_ENABLE;
  hospi1.Init.ChipSelectBoundary = 0;
  hospi1.Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_BYPASSED;
  hospi1.Init.MaxTran = 0;
  hospi1.Init.Refresh = 400;
  if (HAL_OSPI_Init(&hospi1) != HAL_OK)
  {
    Error_Handler();
  }
  sOspiManagerCfg.ClkPort = 1;
  sOspiManagerCfg.DQSPort = 1;
  sOspiManagerCfg.NCSPort = 1;
  sOspiManagerCfg.IOLowPort = HAL_OSPIM_IOPORT_1_LOW;
  sOspiManagerCfg.IOHighPort = HAL_OSPIM_IOPORT_1_HIGH;
  if (HAL_OSPIM_Config(&hospi1, &sOspiManagerCfg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  sHyperBusCfg.RWRecoveryTime = 3;
  sHyperBusCfg.AccessTime = 4;
  sHyperBusCfg.WriteZeroLatency = HAL_OSPI_LATENCY_ON_WRITE;
  sHyperBusCfg.LatencyMode = HAL_OSPI_FIXED_LATENCY;
  if (HAL_OSPI_HyperbusCfg(&hospi1, &sHyperBusCfg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OCTOSPI1_Init 2 */
  /* Memory-mapped mode configuration --------------------------------------- */
  sCommand.AddressSpace = HAL_OSPI_MEMORY_ADDRESS_SPACE;
  sCommand.AddressSize  = HAL_OSPI_ADDRESS_32_BITS;
  sCommand.DQSMode      = HAL_OSPI_DQS_ENABLE;
  sCommand.Address      = 0;
  sCommand.NbData       = 1;
  if (HAL_OSPI_HyperbusCmd(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
      Error_Handler();

  sMemMappedCfg.TimeOutActivation = HAL_OSPI_TIMEOUT_COUNTER_DISABLE;
  if (HAL_OSPI_MemoryMapped(&hospi1, &sMemMappedCfg) != HAL_OK)
      Error_Handler();

  uint32_t TmpCcr  = hospi1.Instance->CCR;
  uint32_t TmpWccr = hospi1.Instance->WCCR;

  MODIFY_REG(hospi1.Instance->DCR1, OCTOSPI_DCR1_MTYP, OCTOSPI_DCR1_MTYP_2|OCTOSPI_DCR1_MTYP_0);
  while (hospi1.Instance->SR & OCTOSPI_SR_BUSY);
  hospi1.Instance->CCR  = TmpCcr;
  hospi1.Instance->WCCR = TmpWccr;
  while (hospi1.Instance->SR & OCTOSPI_SR_BUSY);

  MODIFY_REG(hospi1.Instance->HLCR, OCTOSPI_HLCR_WZL,  OCTOSPI_HLCR_WZL);
  // configure HyperRam = fixed latency, 4 CLK initial latency, 32 bytes burst, 34 ohm drive strength
  *(__IO uint16_t*)0x90001000 = 0x8FFF;
  // wait write to be completed before abort
  HAL_Delay(2);

  MODIFY_REG(hospi1.Instance->CR, OCTOSPI_CR_ABORT, OCTOSPI_CR_ABORT);
  while ((hospi1.Instance->SR & OCTOSPI_SR_BUSY) || (hospi1.Instance->CR & OCTOSPI_CR_ABORT));
  MODIFY_REG(hospi1.Instance->HLCR, OCTOSPI_HLCR_WZL,  0);
  MODIFY_REG(hospi1.Instance->DCR1, OCTOSPI_DCR1_MTYP, OCTOSPI_DCR1_MTYP_2);
  hospi1.Instance->CCR  = TmpCcr;
  hospi1.Instance->WCCR = TmpWccr;

  // Write and check a word
  *(__IO uint32_t*)0x90009000 = 0xCAFEFACE;
  if (*(__IO uint32_t*)0x90009000 != 0xCAFEFACE)	  DevStatus[DEVSTATUS_HYPERRAM_STATUS_IDX] = DEVSTATUS_HYPERRAM_FAIL;
  DevStatus_SetInitPhase(DEVSTATUS_UNKNOWN_INIT_VALUE);
  /* USER CODE END OCTOSPI1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
  if (Rtc_GetRtcTimeSetStatus())  return;	//already initialized
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the WakeUp
  */
  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SDMMC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDMMC2_SD_Init(void)
{

  /* USER CODE BEGIN SDMMC2_Init 0 */

  /* USER CODE END SDMMC2_Init 0 */

  /* USER CODE BEGIN SDMMC2_Init 1 */

  /* USER CODE END SDMMC2_Init 1 */
  hsd2.Instance = SDMMC2;
  hsd2.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd2.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd2.Init.BusWide = SDMMC_BUS_WIDE_4B;
  hsd2.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;
  hsd2.Init.ClockDiv = 20;
  /* USER CODE BEGIN SDMMC2_Init 2 */
  SDMMC_PowerState_Cycle(SDMMC2);
  /* USER CODE END SDMMC2_Init 2 */

}

/**
  * @brief SPI6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI6_Init(void)
{

  /* USER CODE BEGIN SPI6_Init 0 */

  /* USER CODE END SPI6_Init 0 */

  /* USER CODE BEGIN SPI6_Init 1 */

  /* USER CODE END SPI6_Init 1 */
  /* SPI6 parameter configuration*/
  hspi6.Instance = SPI6;
  hspi6.Init.Mode = SPI_MODE_MASTER;
  hspi6.Init.Direction = SPI_DIRECTION_2LINES_TXONLY;
  hspi6.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi6.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi6.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi6.Init.NSS = SPI_NSS_SOFT;
  hspi6.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi6.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi6.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi6.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi6.Init.CRCPolynomial = 0x0;
  hspi6.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  hspi6.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi6.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi6.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi6.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi6.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi6.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi6.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi6.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi6.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI6_Init 2 */

  /* USER CODE END SPI6_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 139;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 0;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */
  __HAL_TIM_SET_AUTORELOAD(&htim5, 0xFFFFFFFF);
  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief UART7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART7_Init(void)
{

  /* USER CODE BEGIN UART7_Init 0 */

  /* USER CODE END UART7_Init 0 */

  /* USER CODE BEGIN UART7_Init 1 */

  /* USER CODE END UART7_Init 1 */
  huart7.Instance = UART7;
  huart7.Init.BaudRate = 3000000;
  huart7.Init.WordLength = UART_WORDLENGTH_8B;
  huart7.Init.StopBits = UART_STOPBITS_1;
  huart7.Init.Parity = UART_PARITY_NONE;
  huart7.Init.Mode = UART_MODE_TX_RX;
  huart7.Init.HwFlowCtl = UART_HWCONTROL_CTS;
  huart7.Init.OverSampling = UART_OVERSAMPLING_16;
  huart7.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart7.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart7.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart7) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart7, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart7, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart7) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART7_Init 2 */

  /* USER CODE END UART7_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART10 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART10_UART_Init(void)
{

  /* USER CODE BEGIN USART10_Init 0 */

  /* USER CODE END USART10_Init 0 */

  /* USER CODE BEGIN USART10_Init 1 */

  /* USER CODE END USART10_Init 1 */
  huart10.Instance = USART10;
  huart10.Init.BaudRate = 9600;
  huart10.Init.WordLength = UART_WORDLENGTH_8B;
  huart10.Init.StopBits = UART_STOPBITS_1;
  huart10.Init.Parity = UART_PARITY_NONE;
  huart10.Init.Mode = UART_MODE_TX_RX;
  huart10.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart10.Init.OverSampling = UART_OVERSAMPLING_16;
  huart10.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart10.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart10.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart10) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart10, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart10, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart10) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART10_Init 2 */

  /* USER CODE END USART10_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

}

/**
  * Enable MDMA controller clock
  * Configure MDMA for global transfers
  *   hmdma_mdma_channel40_sw_0
  *   hmdma_mdma_channel41_sw_0
  *   hmdma_mdma_channel42_sw_0
  *   hmdma_mdma_channel43_sw_0
  */
static void MX_MDMA_Init(void)
{

  /* MDMA controller clock enable */
  __HAL_RCC_MDMA_CLK_ENABLE();
  /* Local variables */

  /* Configure MDMA channel MDMA_Channel0 */
  /* Configure MDMA request hmdma_mdma_channel40_sw_0 on MDMA_Channel0 */
  hmdma_mdma_channel40_sw_0.Instance = MDMA_Channel0;
  hmdma_mdma_channel40_sw_0.Init.Request = MDMA_REQUEST_SW;
  hmdma_mdma_channel40_sw_0.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
  hmdma_mdma_channel40_sw_0.Init.Priority = MDMA_PRIORITY_LOW;
  hmdma_mdma_channel40_sw_0.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
  hmdma_mdma_channel40_sw_0.Init.SourceInc = MDMA_SRC_INC_WORD;
  hmdma_mdma_channel40_sw_0.Init.DestinationInc = MDMA_DEST_INC_WORD;
  hmdma_mdma_channel40_sw_0.Init.SourceDataSize = MDMA_SRC_DATASIZE_WORD;
  hmdma_mdma_channel40_sw_0.Init.DestDataSize = MDMA_DEST_DATASIZE_WORD;
  hmdma_mdma_channel40_sw_0.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
  hmdma_mdma_channel40_sw_0.Init.BufferTransferLength = 128;
  hmdma_mdma_channel40_sw_0.Init.SourceBurst = MDMA_SOURCE_BURST_32BEATS;
  hmdma_mdma_channel40_sw_0.Init.DestBurst = MDMA_DEST_BURST_32BEATS;
  hmdma_mdma_channel40_sw_0.Init.SourceBlockAddressOffset = 0;
  hmdma_mdma_channel40_sw_0.Init.DestBlockAddressOffset = 0;
  if (HAL_MDMA_Init(&hmdma_mdma_channel40_sw_0) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure MDMA channel MDMA_Channel1 */
  /* Configure MDMA request hmdma_mdma_channel41_sw_0 on MDMA_Channel1 */
  hmdma_mdma_channel41_sw_0.Instance = MDMA_Channel1;
  hmdma_mdma_channel41_sw_0.Init.Request = MDMA_REQUEST_SW;
  hmdma_mdma_channel41_sw_0.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
  hmdma_mdma_channel41_sw_0.Init.Priority = MDMA_PRIORITY_LOW;
  hmdma_mdma_channel41_sw_0.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
  hmdma_mdma_channel41_sw_0.Init.SourceInc = MDMA_SRC_INC_WORD;
  hmdma_mdma_channel41_sw_0.Init.DestinationInc = MDMA_DEST_INC_WORD;
  hmdma_mdma_channel41_sw_0.Init.SourceDataSize = MDMA_SRC_DATASIZE_WORD;
  hmdma_mdma_channel41_sw_0.Init.DestDataSize = MDMA_DEST_DATASIZE_WORD;
  hmdma_mdma_channel41_sw_0.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
  hmdma_mdma_channel41_sw_0.Init.BufferTransferLength = 128;
  hmdma_mdma_channel41_sw_0.Init.SourceBurst = MDMA_SOURCE_BURST_32BEATS;
  hmdma_mdma_channel41_sw_0.Init.DestBurst = MDMA_DEST_BURST_32BEATS;
  hmdma_mdma_channel41_sw_0.Init.SourceBlockAddressOffset = 0;
  hmdma_mdma_channel41_sw_0.Init.DestBlockAddressOffset = 0;
  if (HAL_MDMA_Init(&hmdma_mdma_channel41_sw_0) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure MDMA channel MDMA_Channel15 */
  /* Configure MDMA request hmdma_mdma_channel42_sw_0 on MDMA_Channel15 */
  hmdma_mdma_channel42_sw_0.Instance = MDMA_Channel15;
  hmdma_mdma_channel42_sw_0.Init.Request = MDMA_REQUEST_SW;
  hmdma_mdma_channel42_sw_0.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
  hmdma_mdma_channel42_sw_0.Init.Priority = MDMA_PRIORITY_LOW;
  hmdma_mdma_channel42_sw_0.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
  hmdma_mdma_channel42_sw_0.Init.SourceInc = MDMA_SRC_INC_WORD;
  hmdma_mdma_channel42_sw_0.Init.DestinationInc = MDMA_DEST_INC_WORD;
  hmdma_mdma_channel42_sw_0.Init.SourceDataSize = MDMA_SRC_DATASIZE_WORD;
  hmdma_mdma_channel42_sw_0.Init.DestDataSize = MDMA_DEST_DATASIZE_WORD;
  hmdma_mdma_channel42_sw_0.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
  hmdma_mdma_channel42_sw_0.Init.BufferTransferLength = 128;
  hmdma_mdma_channel42_sw_0.Init.SourceBurst = MDMA_SOURCE_BURST_32BEATS;
  hmdma_mdma_channel42_sw_0.Init.DestBurst = MDMA_DEST_BURST_32BEATS;
  hmdma_mdma_channel42_sw_0.Init.SourceBlockAddressOffset = 0;
  hmdma_mdma_channel42_sw_0.Init.DestBlockAddressOffset = 0;
  if (HAL_MDMA_Init(&hmdma_mdma_channel42_sw_0) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure MDMA channel MDMA_Channel2 */
  /* Configure MDMA request hmdma_mdma_channel43_sw_0 on MDMA_Channel2 */
  hmdma_mdma_channel43_sw_0.Instance = MDMA_Channel2;
  hmdma_mdma_channel43_sw_0.Init.Request = MDMA_REQUEST_SW;
  hmdma_mdma_channel43_sw_0.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
  hmdma_mdma_channel43_sw_0.Init.Priority = MDMA_PRIORITY_LOW;
  hmdma_mdma_channel43_sw_0.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
  hmdma_mdma_channel43_sw_0.Init.SourceInc = MDMA_SRC_INC_DISABLE;
  hmdma_mdma_channel43_sw_0.Init.DestinationInc = MDMA_DEST_INC_WORD;
  hmdma_mdma_channel43_sw_0.Init.SourceDataSize = MDMA_SRC_DATASIZE_WORD;
  hmdma_mdma_channel43_sw_0.Init.DestDataSize = MDMA_DEST_DATASIZE_WORD;
  hmdma_mdma_channel43_sw_0.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
  hmdma_mdma_channel43_sw_0.Init.BufferTransferLength = 128;
  hmdma_mdma_channel43_sw_0.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;
  hmdma_mdma_channel43_sw_0.Init.DestBurst = MDMA_DEST_BURST_32BEATS;
  hmdma_mdma_channel43_sw_0.Init.SourceBlockAddressOffset = 0;
  hmdma_mdma_channel43_sw_0.Init.DestBlockAddressOffset = 0;
  if (HAL_MDMA_Init(&hmdma_mdma_channel43_sw_0) != HAL_OK)
  {
    Error_Handler();
  }

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, EN_12V_SW_Pin|NFC_LDO_DUMMY_Pin|VCC_NFC_Pin|EN_3V3AUX_Pin
                          |EN_BUZ_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MODEM_RESET_GPIO_Port, MODEM_RESET_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(EN_3V3_GPIO_Port, EN_3V3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOH, EN_6V_Pin|RS485_DE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(EN_LED_IR_GPIO_Port, EN_LED_IR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_PWM_GPIO_Port, LED_PWM_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OCTOSPIM_RST_GPIO_Port, OCTOSPIM_RST_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, MOTOR2_SLP_Pin|MOTOR_EN_Pin|MOTOR2_PH_Pin|EN_12V_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, MOTOR1_SLP_Pin|MOTOR1_PH_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CAMERA_TRIGGER_GPIO_Port, CAMERA_TRIGGER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, CAMERA_RESET_Pin|CAMERA_STBY_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(EN_3V3S_GPIO_Port, EN_3V3S_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : EN_12V_SW_Pin NFC_LDO_DUMMY_Pin VCC_NFC_Pin EN_3V3AUX_Pin
                           EN_BUZ_Pin */
  GPIO_InitStruct.Pin = EN_12V_SW_Pin|NFC_LDO_DUMMY_Pin|VCC_NFC_Pin|EN_3V3AUX_Pin
                          |EN_BUZ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : MODEM_RESET_Pin */
  GPIO_InitStruct.Pin = MODEM_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MODEM_RESET_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : EN_3V3_Pin */
  GPIO_InitStruct.Pin = EN_3V3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(EN_3V3_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : EN_6V_Pin */
  GPIO_InitStruct.Pin = EN_6V_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(EN_6V_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : RS485_DE_Pin */
  GPIO_InitStruct.Pin = RS485_DE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(RS485_DE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TILT_Pin */
  GPIO_InitStruct.Pin = TILT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(TILT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : EN_LED_IR_Pin */
  GPIO_InitStruct.Pin = EN_LED_IR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(EN_LED_IR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_PWM_Pin */
  GPIO_InitStruct.Pin = LED_PWM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_PWM_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : RS485_RE_Pin */
  GPIO_InitStruct.Pin = RS485_RE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(RS485_RE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : OCTOSPIM_RST_Pin */
  GPIO_InitStruct.Pin = OCTOSPIM_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(OCTOSPIM_RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : NFC_INT_Pin */
  GPIO_InitStruct.Pin = NFC_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(NFC_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : MOTOR2_SLP_Pin MOTOR_EN_Pin MOTOR2_PH_Pin EN_12V_Pin */
  GPIO_InitStruct.Pin = MOTOR2_SLP_Pin|MOTOR_EN_Pin|MOTOR2_PH_Pin|EN_12V_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : MOTOR1_SLP_Pin MOTOR1_PH_Pin EN_3V3S_Pin */
  GPIO_InitStruct.Pin = MOTOR1_SLP_Pin|MOTOR1_PH_Pin|EN_3V3S_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : FLASH_Pin */
  GPIO_InitStruct.Pin = FLASH_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(FLASH_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CAMERA_TRIGGER_Pin */
  GPIO_InitStruct.Pin = CAMERA_TRIGGER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(CAMERA_TRIGGER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CAMERA_RESET_Pin CAMERA_STBY_Pin */
  GPIO_InitStruct.Pin = CAMERA_RESET_Pin|CAMERA_STBY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : CAMERA_EXTCLK_Pin */
  GPIO_InitStruct.Pin = CAMERA_EXTCLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
  HAL_GPIO_Init(CAMERA_EXTCLK_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    __asm volatile
    (
    	" mov  r4, #0xA5A5                \n"
        " push {r4}                       \n"
        " push {r4}                       \n"
        " push {lr}                       \n"
        " push {r12}                      \n"
        " push {r3}                       \n"
    	" push {r2}                       \n"
    	" push {r1}                       \n"
    	" push {r0}                       \n"
    	" mov  r0, sp                     \n"
        " b DevStatus_SaveStackAndReset   \n"
    );
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
