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
    HAL_GPIO_WritePin(SPI_CSN_GPIO_Port,SPI_CSN_Pin,GPIO_PIN_RESET);
}

void radio_hal_SetNsel(void)
{
//    RF_NSEL = 1;
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
    printf("radio SPI_ReadWrite Error (%d)!!\n", status);
  }

  while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY){}
  
}


U8 SpiReadWrite( U8 byteToWrite )
{
	U8 wdata[64];
	U8 rdata[64];

	uint16_t byteCount;

	wdata[0] = byteToWrite;	//	0xFF;
	rdata[0] = 0;

	byteCount = 1;

	HAL_StatusTypeDef status;

	status = HAL_SPI_TransmitReceive_DMA(&hspi1, wdata, rdata, byteCount);
	if(status != HAL_OK)
	{
		printf("radio SPI_ReadWrite Error (%d)!!\n", status);
	}

	while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY){}

	return rdata[0];
}


U8 *SpiWriteData( U16 byteCount, U8 *byteToWrite )
{
	U8 rdata[64];
	U8 *wdata = byteToWrite;

	rdata[0] = 0;

	memset( rdata, 0, sizeof(rdata) );

	HAL_StatusTypeDef status;

	status = HAL_SPI_TransmitReceive_DMA(&hspi1, wdata, rdata, byteCount);
	if(status != HAL_OK)
	{
		printf("radio SPI_ReadWrite Error (%d)!!\n", status);
	}

	while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY){}

	return byteToWrite;
}


U8 *SpiReadData( U16 byteCount, U8 *byteToWrite )
{

	U8 wdata[64];
	U8 *rdata = byteToWrite;

	memset(wdata, 0xFF, byteCount);

	HAL_StatusTypeDef status;

	status = HAL_SPI_TransmitReceive_DMA(&hspi1, wdata, rdata, byteCount);
	if(status != HAL_OK)
	{
		printf("radio SPI_ReadWrite Error (%d)!!\n", status);
	}

	while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY){}

	return byteToWrite;
}

void radio_hal_SpiWriteByte(U8 byteToWrite)
{
#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB)
  bSpi_ReadWriteSpi1(byteToWrite);
#else
  SpiReadWrite(byteToWrite);
#endif
}

U8 radio_hal_SpiReadByte(void)
{
#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB)
  return bSpi_ReadWriteSpi1(0xFF);
#else
  return SpiReadWrite(0xFF);
#endif
}

void radio_hal_SpiWriteData(U8 byteCount, U8* pData)
{
#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB)
  vSpi_WriteDataSpi1(byteCount, pData);
#else
  SpiWriteData(byteCount, pData);
#endif
}

void radio_hal_SpiReadData(U8 byteCount, U8* pData)
{
#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB)
  vSpi_ReadDataSpi1(byteCount, pData);
#else
  SpiReadData(byteCount, pData);
#endif
}

#ifdef RADIO_DRIVER_EXTENDED_SUPPORT
BIT radio_hal_Gpio0Level(void)
{
  BIT retVal = FALSE;

#ifdef SILABS_PLATFORM_DKMB
  retVal = FALSE;
#endif
#ifdef SILABS_PLATFORM_UDP
  retVal = EZRP_RX_DOUT;
#endif
#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB)
  retVal = RF_GPIO0;
#endif
#if (defined SILABS_PLATFORM_WMB930)
  retVal = FALSE;
#endif
#if defined (SILABS_PLATFORM_WMB912)
  #ifdef SILABS_IO_WITH_EXTENDER
    //TODO
    retVal = FALSE;
  #endif
#endif

  return retVal;
}

BIT radio_hal_Gpio1Level(void)
{
	return HAL_GPIO_ReadPin(GPIO1_GPIO_Port,GPIO1_Pin);

  BIT retVal = FALSE;

#ifdef SILABS_PLATFORM_DKMB
  retVal = FSK_CLK_OUT;
#endif
#ifdef SILABS_PLATFORM_UDP
  retVal = FALSE; //No Pop
#endif
#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB930)
  retVal = RF_GPIO1;
#endif
#if defined (SILABS_PLATFORM_WMB912)
  #ifdef SILABS_IO_WITH_EXTENDER
    //TODO
    retVal = FALSE;
  #endif
#endif

  return retVal;
}

BIT radio_hal_Gpio2Level(void)
{
  BIT retVal = FALSE;

#ifdef SILABS_PLATFORM_DKMB
  retVal = DATA_NFFS;
#endif
#ifdef SILABS_PLATFORM_UDP
  retVal = FALSE; //No Pop
#endif
#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB930)
  retVal = RF_GPIO2;
#endif
#if defined (SILABS_PLATFORM_WMB912)
  #ifdef SILABS_IO_WITH_EXTENDER
    //TODO
    retVal = FALSE;
  #endif
#endif

  return retVal;
}

BIT radio_hal_Gpio3Level(void)
{
  BIT retVal = FALSE;

#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB930)
  retVal = RF_GPIO3;
#elif defined (SILABS_PLATFORM_WMB912)
  #ifdef SILABS_IO_WITH_EXTENDER
    //TODO
    retVal = FALSE;
  #endif
#endif

  return retVal;
}

#endif
