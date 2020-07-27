/*!
 * File:
 *  radio_hal.c
 *
 * Description:
 *  This file contains RADIO HAL.
 *
 * Silicon Laboratories Confidential
 * Copyright 2011 Silicon Laboratories, Inc.
 */

                /* ======================================= *
                 *              I N C L U D E              *
                 * ======================================= */

//#include "..\..\bsp.h"

#include "main.h"
#include "radio_hal.h"

#include "stm32f4xx_hal.h"		//	GPIO_PinState
                /* ======================================= *
                 *          D E F I N I T I O N S          *
                 * ======================================= */

                /* ======================================= *
                 *     G L O B A L   V A R I A B L E S     *
                 * ======================================= */

                /* ======================================= *
                 *      L O C A L   F U N C T I O N S      *
                 * ======================================= */

                /* ======================================= *
                 *     P U B L I C   F U N C T I O N S     *
                 * ======================================= */


void radio_hal_AssertShutdown(void)
{
//  PWRDN = 1;
	/*
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3|GPIO_PIN_5, GPIO_PIN_SET);
	/*/
//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
  HAL_GPIO_WritePin(PWR_RF_GPIO_Port, PWR_RF_Pin, GPIO_PIN_SET);
  HAL_Delay(1);
  HAL_GPIO_WritePin(TRN_RST_GPIO_Port,TRN_RST_Pin,GPIO_PIN_SET);
	//	*/
}

void radio_hal_DeassertShutdown(void)
{
//  PWRDN = 0;
	/*
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3|GPIO_PIN_5, GPIO_PIN_RESET);
	/*/
//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TRN_RST_GPIO_Port,TRN_RST_Pin,GPIO_PIN_RESET);
	//	*/
}

void radio_hal_ClearNsel(void)
{
//    RF_NSEL = 0;
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SPI_CSN_GPIO_Port,SPI_CSN_Pin,GPIO_PIN_RESET);
}

void radio_hal_SetNsel(void)
{
//    RF_NSEL = 1;
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
//    HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_SET);
     HAL_GPIO_WritePin(SPI_CSN_GPIO_Port,SPI_CSN_Pin,GPIO_PIN_SET);
}

BIT radio_hal_NirqLevel(void)
{
//	return (GPIO_PinState) RF_NIRQ;
    return HAL_GPIO_ReadPin(RF_INT_GPIO_Port,RF_INT_Pin);
}

U8 spi_wbyte[64];
U8 spi_rbyte[64];

void radio_SPI_ReadWrite(U8 byteCount, U8* wdata, U8* rdata)
{
  HAL_StatusTypeDef status;

  status = HAL_SPI_TransmitReceive_DMA(&hspi1, wdata, rdata, byteCount);
  if(status != HAL_OK)
  {
    printf("radio SPI_ReadWrite error (%d)!!\n", status);
  }

  while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY){}
  
}

void radio_hal_SpiWriteByte(U8 byteToWrite)
{
#if 1
  spi_wbyte[0] = byteToWrite;
  spi_rbyte[0] = 0;
  
  (void)radio_SPI_ReadWrite(1, spi_wbyte, spi_rbyte);
#else
    //U8 rx_dat;
  if (HAL_SPI_Transmit(&hspi1,&byteToWrite,1, 10) != HAL_OK)
  //if (HAL_SPI_TransmitReceive(&hspi1,&byteToWrite,&rx_dat,1,10) != HAL_OK)
  {
    printf("Spiwritebyte error !!\n");
    //Error_Handler();
  }
#endif
}


U8 radio_hal_SpiReadByte(void)
{
#if 1
  spi_wbyte[0] = 0xFF;
  spi_rbyte[0] = 0;
  (void)radio_SPI_ReadWrite(1, spi_wbyte, spi_rbyte);
  return spi_rbyte[0];
#else
  U8 rx_dat;
  //U8 tx_dat=0;
  if (HAL_SPI_Receive(&hspi1,&rx_dat,1,10) != HAL_OK)
  //if (HAL_SPI_TransmitReceive(&hspi1,&tx_dat,&rx_dat,1,10) != HAL_OK)
  {
    printf("Spireadbyte error !!\n");
    //Error_Handler();
  }
  return rx_dat;
#endif
}

void radio_hal_SpiWriteData(U8 byteCount, U8* pData)
{
#if 1
    int i;
    for(i=0;i<byteCount;i++)
    {
      spi_wbyte[i] = *(pData+i);
      spi_rbyte[i] = 0;
    }
    radio_SPI_ReadWrite(byteCount, spi_wbyte, spi_rbyte);
#else
  //U8 rx_dat[16];
  if (HAL_SPI_Transmit(&hspi1,pData,byteCount, 10) != HAL_OK)
  //if (HAL_SPI_TransmitReceive(&hspi1,pData,rx_dat,byteCount,10) != HAL_OK)
  {
    printf("SpiWriteData error !!\n");
    //Error_Handler();
  }
#endif
}

void radio_hal_SpiReadData(U8 byteCount, U8* pData)
{
#if 1
  int i;
  for(i=0; i<byteCount; i++)
  {
    spi_wbyte[i] = 0xFF;
    spi_rbyte[i] = 0;
  }
  radio_SPI_ReadWrite(byteCount, spi_wbyte, spi_rbyte);

  for(i=0;i<byteCount;i++)
  {
    *(pData+i) = spi_rbyte[i];
  }
#else
  U8 tx_dat[16];
  //memset(tx_dat,0xFF,16);
  memset(tx_dat,0,8);
  if (HAL_SPI_Receive(&hspi1,pData,byteCount,10) != HAL_OK)
  //if (HAL_SPI_TransmitReceive(&hspi1,tx_dat,pData,byteCount,10) != HAL_OK)
  {
    printf("SpiReadData error !!\n");
    Error_Handler();
  }
#endif
}


#ifdef RADIO_DRIVER_EXTENDED_SUPPORT
GPIO_PinState radio_hal_Gpio0Level(void)
{
  GPIO_PinState retVal = GPIO_PIN_RESET;

  return retVal;
}

GPIO_PinState radio_hal_Gpio1Level(void)
{
  GPIO_PinState retVal = GPIO_PIN_RESET;

	retVal = HAL_GPIO_ReadPin(GPIO1_GPIO_Port,GPIO1_Pin);
  return retVal;
}



GPIO_PinState radio_hal_Gpio2Level(void)
{
  GPIO_PinState retVal = GPIO_PIN_RESET;

	retVal = HAL_GPIO_ReadPin(RF_RX_GPIO_Port,RF_RX_Pin);
  return retVal;
}

GPIO_PinState radio_hal_Gpio3Level(void)
{
  GPIO_PinState retVal = GPIO_PIN_RESET;

	retVal = HAL_GPIO_ReadPin(RF_TX_GPIO_Port,RF_TX_Pin);
  return retVal;
}

#endif
