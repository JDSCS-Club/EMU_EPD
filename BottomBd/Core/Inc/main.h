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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
//#include "retarget.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

extern I2C_HandleTypeDef hi2c2;

extern UART_HandleTypeDef huart5;		//	RS485
extern UART_HandleTypeDef huart1;		//	Console
extern UART_HandleTypeDef huart2;		//	RFM
extern UART_HandleTypeDef huart3;		//	RS485


/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MASTER_IN_Pin GPIO_PIN_13
#define MASTER_IN_GPIO_Port GPIOC
#define CHARGER_DET_Pin GPIO_PIN_14
#define CHARGER_DET_GPIO_Port GPIOC
#define VCC_IN_Pin GPIO_PIN_0
#define VCC_IN_GPIO_Port GPIOC
#define NTC_Pin GPIO_PIN_1
#define NTC_GPIO_Port GPIOC
#define BAT_VCC_Pin GPIO_PIN_2
#define BAT_VCC_GPIO_Port GPIOC
#define RE_Pin GPIO_PIN_3
#define RE_GPIO_Port GPIOC
#define RE1_Pin GPIO_PIN_0
#define RE1_GPIO_Port GPIOA
#define DI_CTL_Pin GPIO_PIN_1
#define DI_CTL_GPIO_Port GPIOA
#define LIGHT_ON_Pin GPIO_PIN_4
#define LIGHT_ON_GPIO_Port GPIOC
#define ST_BY_Pin GPIO_PIN_5
#define ST_BY_GPIO_Port GPIOC
#define POP_UP_Pin GPIO_PIN_0
#define POP_UP_GPIO_Port GPIOB
#define AUDIO_ON_Pin GPIO_PIN_1
#define AUDIO_ON_GPIO_Port GPIOB
#define SDL2_Pin GPIO_PIN_10
#define SDL2_GPIO_Port GPIOB
#define SDA2_Pin GPIO_PIN_11
#define SDA2_GPIO_Port GPIOB
#define DEV_ID3_Pin GPIO_PIN_12
#define DEV_ID3_GPIO_Port GPIOB
#define DEV_ID2_Pin GPIO_PIN_13
#define DEV_ID2_GPIO_Port GPIOB
#define DEV_ID1_Pin GPIO_PIN_14
#define DEV_ID1_GPIO_Port GPIOB
#define DEV_ID0_Pin GPIO_PIN_15
#define DEV_ID0_GPIO_Port GPIOB
#define SD_Pin GPIO_PIN_7
#define SD_GPIO_Port GPIOC
#define MUTE_Pin GPIO_PIN_8
#define MUTE_GPIO_Port GPIOC
#define LED_CTL_Pin GPIO_PIN_9
#define LED_CTL_GPIO_Port GPIOC
#define OVERRIDE_Pin GPIO_PIN_12
#define OVERRIDE_GPIO_Port GPIOA
#define LED_75_Pin GPIO_PIN_3
#define LED_75_GPIO_Port GPIOB
#define RF_LED_Pin GPIO_PIN_5
#define RF_LED_GPIO_Port GPIOB
#define LED_100_RED_Pin GPIO_PIN_6
#define LED_100_RED_GPIO_Port GPIOB
#define LED_100_GREEN_Pin GPIO_PIN_7
#define LED_100_GREEN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
