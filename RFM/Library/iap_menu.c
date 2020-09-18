/**
  ******************************************************************************
  * @file    IAP/IAP_Main/Src/menu.c 
  * @author  MCD Application Team

  * @brief   This file provides the software which contains the main menu routine.
  *          The main menu gives the options of:
  *             - downloading a new binary file, 
  *             - uploading internal flash memory,
  *             - executing the binary file already loaded 
  *             - configuring the write protection of the Flash sectors where the 
  *               user loads his binary file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */ 

/** @addtogroup STM32F2xx_IAP_Main
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "iap_common.h"
#include "flash_if.h"
#include "iap_menu.h"
#include "ymodem.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
pFunction JumpToApplication;
extern uint32_t JumpAddress;        //  bootloader.c
uint32_t FlashProtection = 0;
uint8_t aFileName[FILE_NAME_LENGTH];

/* Private function prototypes -----------------------------------------------*/
void SerialDownload(void);
void SerialUpload(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Download a file via serial port
  * @param  None
  * @retval None
  */
void SerialDownload(void)
{
  uint8_t number[11] = {0};
  uint32_t size = 0;
  COM_StatusTypeDef result;

  printf((uint8_t *)"Waiting for the file to be sent ... (press 'a' to abort)\n\r");
  result = Ymodem_Receive( &size );
  if (result == COM_OK)
  {
    printf((uint8_t *)"\n\n\r Programming Completed Successfully!\n\r--------------------------------\r\n Name: ");
    printf(aFileName);
    Int2Str(number, size);
    printf((uint8_t *)"\n\r Size: ");
    printf(number);
    printf((uint8_t *)" Bytes\r\n");
    printf((uint8_t *)"-------------------\n");
  }
  else if (result == COM_LIMIT)
  {
    printf((uint8_t *)"\n\n\rThe image size is higher than the allowed space memory!\n\r");
  }
  else if (result == COM_DATA)
  {
    printf((uint8_t *)"\n\n\rVerification failed!\n\r");
  }
  else if (result == COM_ABORT)
  {
    printf((uint8_t *)"\r\n\nAborted by user.\n\r");
  }
  else
  {
    printf((uint8_t *)"\n\rFailed to receive the file!\n\r");
  }
}

/**
  * @brief  Upload a file via serial port.
  * @param  None
  * @retval None
  */
void SerialUpload(void)
{
  uint8_t status = 0;

  printf((uint8_t *)"\n\n\rSelect Receive File\n\r");

  HAL_UART_Receive(&huart2, &status, 1, RX_TIMEOUT);
  if ( status == CRC16)
  {
    /* Transmit the flash image through ymodem protocol */
    status = Ymodem_Transmit((uint8_t*)ADDR_FLASH_APP, (const uint8_t*)"UploadedFlashImage.bin", USER_FLASH_SIZE);

    if (status != 0)
    {
      printf((uint8_t *)"\n\rError Occurred while Transmitting File\n\r");
    }
    else
    {
      printf((uint8_t *)"\n\rFile uploaded successfully \n\r");
    }
  }
}

/**
  * @brief  Display the Main Menu on HyperTerminal
  * @param  None
  * @retval None
  */
void IAP_Menu(void)
{
  uint8_t key = 0;

  printf((uint8_t *)"\r\n======================================================================");
  printf((uint8_t *)"\r\n=              (C) COPYRIGHT 2018 STMicroelectronics                 =");
  printf((uint8_t *)"\r\n=                                                                    =");
  printf((uint8_t *)"\r\n=          STM32F2xx In-Application Programming Application          =");
  printf((uint8_t *)"\r\n=                                                                    =");
  printf((uint8_t *)"\r\n=                       By MCD Application Team                      =");
  printf((uint8_t *)"\r\n======================================================================");
  printf((uint8_t *)"\r\n\r\n");

  while (1)
  {

    /* Test if any sector of Flash memory where user application will be loaded is write protected */
    FlashProtection = FLASH_If_GetWriteProtectionStatus();
    
    printf((uint8_t *)"\r\n=================== Main Menu ============================\r\n\n");
    printf((uint8_t *)"  Download image to the internal Flash ----------------- 1\r\n\n");
    printf((uint8_t *)"  Upload image from the internal Flash ----------------- 2\r\n\n");
    printf((uint8_t *)"  Execute the loaded application ----------------------- 3\r\n\n");

    if(FlashProtection != FLASHIF_PROTECTION_NONE)
    {
      printf((uint8_t *)"  Disable the write protection ------------------------- 4\r\n\n");
    }
    else
    {
      printf((uint8_t *)"  Enable the write protection -------------------------- 4\r\n\n");
    }
    printf((uint8_t *)"==========================================================\r\n\n");

    /* Clean the input path */
    __HAL_UART_FLUSH_DRREGISTER(&huart2);
	
    /* Receive key */
//    HAL_UART_Receive(&huart1, &key, 1, RX_TIMEOUT);
    key = getchar();

    switch (key)
    {
    case '1' :
      /* Download user application in the Flash */
      SerialDownload();
      break;
    case '2' :
      /* Upload user application from the Flash */
      SerialUpload();
      break;
    case '3' :
      printf((uint8_t *)"Start program execution......\r\n\n");
      /* execute the new program */
      JumpAddress = *(__IO uint32_t*) (ADDR_FLASH_APP + 4);
      /* Jump to user application */
      JumpToApplication = (pFunction) JumpAddress;

      //========================================================================
      //    Disable Interrupt & DeInit HAL Driver
      vPortEnterCritical();
      HAL_DeInit();
      //========================================================================

      /* Initialize user application's Stack Pointer */
#if defined( USE_YMODEM_EXT_FLASH )
      __set_MSP( *( __IO uint32_t* ) ADDR_FLASH_BOOT );
#else
      __set_MSP(*(__IO uint32_t*)ADDR_FLASH_APP);
#endif

      JumpToApplication();
      break;
    case '4' :
      if (FlashProtection != FLASHIF_PROTECTION_NONE)
      {
        /* Disable the write protection */
        if (FLASH_If_WriteProtectionConfig(OB_WRPSTATE_DISABLE) == HAL_OK)
        {
          printf((uint8_t *)"Write Protection disabled...\r\n");
          printf((uint8_t *)"System will now restart...\r\n");
          /* Launch the option byte loading */
          HAL_FLASH_OB_Launch();
          /* Ulock the flash */
          HAL_FLASH_Unlock();
        }
        else
        {
          printf((uint8_t *)"Error: Flash write un-protection failed...\r\n");
        }
      }
      else
      {
        if (FLASH_If_WriteProtectionConfig(OB_WRPSTATE_ENABLE) == HAL_OK)
        {
          printf((uint8_t *)"Write Protection enabled...\r\n");
          printf((uint8_t *)"System will now restart...\r\n");
          /* Launch the option byte loading */
          HAL_FLASH_OB_Launch();
        }
        else
        {
          printf((uint8_t *)"Error: Flash write protection failed...\r\n");
        }
      }
      break;
    default:
	printf((uint8_t *)"Invalid Number ! ==> The number should be either 1, 2, 3 or 4\r");
	break;
    }
  }
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
