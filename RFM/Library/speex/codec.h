/**
  ******************************************************************************
  * @file SpeexVocoder_STM32-SK/inc/main.h 
  * @author  MCD Application Team
  * @version  V2.0.0
  * @date  04/27/2009
  * @brief  main.c header file
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CODEC_H
#define __CODEC_H

/* Includes ------------------------------------------------------------------*/
#if defined(STM32F407xx)
#include "stm32f4xx.h"
#elif defined(STM32F207xx)
#include "stm32f2xx.h"
#else
#include "stm32f10x.h"
#endif
#include "vocoder.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define MALE_VOICE 1
#define REAL_VOICE 2
#define RECORDED_VOICE 3

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /*__CODEC_H */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
