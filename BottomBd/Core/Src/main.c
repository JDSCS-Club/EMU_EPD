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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "cli.h"			//	CLI ( Command Line Interface )

#if defined(USE_BOOTLOADER)
#include "bootloader.h"

#else
#include "naranja_boron.h"

#endif	//	#if defined(USE_BOOTLOADER)

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

#if defined(USE_BOOTLOADER)
#else	//	Application

ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

TIM_HandleTypeDef htim2;

#endif	//	Application

I2C_HandleTypeDef hi2c2;

UART_HandleTypeDef huart5;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_ADC2_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_UART5_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#if defined(USE_BOOTLOADER)

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

//===========================================================================
PUTCHAR_PROTOTYPE
//===========================================================================
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */

	//========================================================================
	//	Console
	HAL_UART_Transmit( &huart1, (uint8_t *)&ch, 1, 0xFFFF );
	if ( ch == '\n' )
	{
		HAL_UART_Transmit( &huart1, (uint8_t *)"\r", 1, 0xFFFF );
	}

	return ch;
}

#endif	//	Bootloader


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_ADC2_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  MX_UART5_Init();
  /* USER CODE BEGIN 2 */

#if defined(USE_BOOTLOADER)
#else	//	Application

  HAL_TIM_Base_Start_IT(&htim2);

#endif

  /*
  RetargetInit(&huart1); // for printf

  /*/

#if defined(USE_BOOTLOADER)

#else	//	Application

  //	UART Interrupt ����.
  //	UART Queue �ʱ�ȭ.
  SerialInitQueue();

//  SerialInit( &huart1, NULL, NULL, NULL );	//	Console Interrupt

  //	UART Interrupt ����.
  SerialInit( &huart1, &huart2, &huart3, &huart5 );
//  SerialInit( &huart1, NULL, NULL, NULL );//&huart2, &huart3, &huart5 );
  InitRS485();		//	Init RS485

#endif

  setbuf ( stdout, NULL );		            //	1024 byte buffer clear
//  setvbuf ( stdout, NULL, _IOLBF, NULL );	//	Line Buffer
  setvbuf ( stdout, NULL, _IONBF, NULL );	//	No Buffer

#if defined(USE_BOOTLOADER)
  printf( "Boot\n" );    // xTaskGetTickCount() );
#else
  printf( "[%d]Start\n", HAL_GetTick() );    // xTaskGetTickCount() );
#endif

  //	*/

//=============================================================================
#if defined(USE_BOOTLOADER)
//=============================================================================

  BootLoaderTask();

//=============================================================================
#else	//	Application
//=============================================================================
  setAmpMute(true);

  TestNVRAM( &hi2c2 );

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  int sCnt = 0;
  
  int nTick;
      
  
  static int s_nTick;
  static int s_nOccCnt;
  static int s_currentCnt;
      
  static int s_RssNgCleanCnt;

  static int s_I2c_Cnt = 0;

  uint8_t     nTbuf[10];
  uint8_t     nRbuf[10];




    printf( "*********OCC Function(ID %02X)*********\n",getRS485Id() );
    printf( "*********Design by DS-Jang (2022-10-18)\n" );
        
    
        
    HAL_StatusTypeDef result;


	result = HAL_I2C_IsDeviceReady( &hi2c2, (uint16_t)(0xD8), 2, 2 );

	if ( HAL_OK == result )
	{
	  printf( "-0xD8 AMP_SEt_OK \n" );
	}
	else
	{
	  printf( "-0xD8 AMP_SEt_NG(%2x)\n",result);
	}


	nRbuf[0] = 0xFF;

	if(MB85_HAL_ReadBytes(&hi2c2,0xD8,0x00,(uint8_t *)nRbuf,1))
	{
		printf( "-0x00 Read Test(%02X) \n",nRbuf[0]);
	}
	else
	{
		printf( "-0x00 Read Test(0x00-0x00) NG \n" );
	}

	nRbuf[0] = 0xFF;

	if(MB85_HAL_ReadBytes(&hi2c2,0xD8,0x01,(uint8_t *)nRbuf,1))
	{
		printf( "-0x01 Read Test(%02X) \n",nRbuf[0] );
	}
	else
	{
		printf( "-0x01 Read Test(0x00-0x01) NG \n" );
	}


//    nTbuf[0] = 0x9F;
//
//    if(MB85_HAL_WriteBytes(&hi2c2,0xD8,0x0C,(uint8_t *)nTbuf,1))
//    {
//        printf("-0xD8-0x9F Write Test(0x0C-0x%02X) OK \n",nTbuf[0] );
//    }
//    else
//    {
//        printf( "-0xD8-0x9F Write Test(0x08-0xbe) NG \n" );
//    }

       

	setAmpSd(true); //AMP �ʱ� �����ϴ� �κ�.

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  	nTick = HAL_GetTick();

		//=============================================================================
		LoopProcCLI();		//	CLI ( Command Line Interface )
		LoopProcRS485_3ch();	//	RS485
        LoopProcRS485_5ch();	//	RS485
        LoopProcRS485_2ch();    // Console
		//=============================================================================

		
		processRfLed();
		processChargeLed();
		processLightLed();
		processAudioAmpProcess();

		processTestDebug();
		//	  Uart3_Process();


        if ( (nTick - s_nOccCnt) >= 100)
        {

#if defined(SIL_RFM)
        	static int s_bMstIn = 0;
        	int bMstIn = getMasterIn();

        	if ( s_bMstIn != bMstIn )
        	{
        		//	Active Low
        		if ( bMstIn )
        		{
        			RFMOccPaStop();
        		}
        		else
        		{
        			RFMOccPaStart();
        		}
        	}
    		s_bMstIn = bMstIn;

#else
            s_nOccCnt = nTick;
                
            // 1?�차 �?10 ?�차�? ?�?�객 ?�작 ?�도�??�정.
            if(getRS485Id() == 0x01 ||
               getRS485Id() == 0x0A )
            {
                processOverrideOn();
            }
                
            ONTD_Function();
#endif
        }
            
                
		if ( (nTick - s_nTick) >= 1000)
		{
//			printf("[%d]\n", nTick);
			s_nTick = nTick;
			processGetBatVol();			//	ADC
            
                
			if(getAmpFault())
			{

				//printf( "getAmpOk \n" );
			}
			else
			{
				//printf( "getAmpFault \n" );
			}

			//=============================================================================
            checkSerial(&huart1);       //  Debug
            checkSerial(&huart2);       //  RFM
            checkSerial(&huart3);       //  RS485 Line üũ.
            checkSerial(&huart5);       //  RS485 Line üũ.
            //=============================================================================
		}

#if defined(SIL_RFM)
		//	SIL - SPK Check Disable
#else
        if ( (nTick - s_currentCnt) >= 5000)
        {
            s_currentCnt = nTick;
                
             //processCurrentVal();
                 
                
             if(uAudioPlayFlag == false)
             {
                 
                if(AMP_SPK_CHECK())
                {
                    if(uSpk_Stat < 6)
                    {
                        uSpk_Stat++;
                    }
                }
             }
            
        }
#endif
        
        if ( (nTick - s_RssNgCleanCnt) >= 15000)
        {
                    
            if(uRssi_NgFlag) // 10초에 ?�번 체크 ?�서 RSSI 고장?? ?�다�?1 감소 ?�다.
            {
               uRssi_NgFlag--;
            }
            
            if(uSpk_Stat)
            {
                uSpk_Stat--;
            }
            
                
             s_RssNgCleanCnt = nTick;
             
        }
                

//		HAL_Delay(1);
  }

//=============================================================================
#endif	//	Application
//=============================================================================

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
        /**Configure the Systick interrupt time 

    */

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);



    /**Configure the Systick 

    */

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);



  /* SysTick_IRQn interrupt configuration */

  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);


          
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

#if defined(USE_BOOTLOADER)
#else	//	Application

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

#endif	//	Application

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

#if defined(USE_BOOTLOADER)
#else	//	Application

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */
  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.ContinuousConvMode = ENABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

#endif	//	Application

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{
    GPIO_InitTypeDef   GPIO_InitStructure;
  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */
 __HAL_RCC_GPIOB_CLK_ENABLE();
  /* USER CODE BEGIN I2C2_Init 1 */
 GPIO_InitStructure.Pin = SDL2_Pin|SDA2_Pin;
 GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
 GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;

 __GPIOB_CLK_ENABLE();
 HAL_GPIO_Init(SDL2_GPIO_Port, &GPIO_InitStructure);
 
 HAL_GPIO_WritePin(SDL2_GPIO_Port, SDL2_Pin, GPIO_PIN_SET);
HAL_GPIO_WritePin(SDL2_GPIO_Port, SDA2_Pin, GPIO_PIN_SET);
    
   // HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
      
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

#if defined(USE_BOOTLOADER)
#else	//	Application

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 8-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1000-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

#endif	//	Application

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 38400;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 38400;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, RE_Pin|SD_Pin|MUTE_Pin|LED_CTL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RE1_Pin|OVERRIDE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DI_CTL_GPIO_Port, DI_CTL_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_75_Pin|RF_LED_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_100_RED_Pin|LED_100_GREEN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : MASTER_IN_Pin CHARGER_DET_Pin VCC_IN_Pin */
  GPIO_InitStruct.Pin = MASTER_IN_Pin|CHARGER_DET_Pin|VCC_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : RE_Pin SD_Pin MUTE_Pin LED_CTL_Pin */
  GPIO_InitStruct.Pin = RE_Pin|SD_Pin|MUTE_Pin|LED_CTL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : RE1_Pin DI_CTL_Pin OVERRIDE_Pin */
  GPIO_InitStruct.Pin = RE1_Pin|DI_CTL_Pin|OVERRIDE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : VCC_AUDIO_IN_Pin VCC_LED_IN_Pin AMP_FAULT_Pin VCC_RF_IN_Pin */
  GPIO_InitStruct.Pin = VCC_AUDIO_IN_Pin|VCC_LED_IN_Pin|AMP_FAULT_Pin|VCC_RF_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LIGHT_ON_Pin ST_BY_Pin */
  GPIO_InitStruct.Pin = LIGHT_ON_Pin|ST_BY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : POP_UP_Pin AUDIO_ON_Pin */
  GPIO_InitStruct.Pin = POP_UP_Pin|AUDIO_ON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : DEV_ID3_Pin DEV_ID2_Pin DEV_ID1_Pin DEV_ID0_Pin */
  GPIO_InitStruct.Pin = DEV_ID3_Pin|DEV_ID2_Pin|DEV_ID1_Pin|DEV_ID0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_75_Pin RF_LED_Pin LED_100_RED_Pin LED_100_GREEN_Pin */
  GPIO_InitStruct.Pin = LED_75_Pin|RF_LED_Pin|LED_100_RED_Pin|LED_100_GREEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
  /* User can add his own implementation to report the HAL error return state */

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
