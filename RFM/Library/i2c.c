/* | -------------------------------- R2T Team Libraries -------------------------
 * | @Created On Sep,11,2015
 * | @File Name : at24_hal_i2c
 * | @Brief : STM32 HAL Driver for AT24 eeprom series
 * | @Copyright :
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * |
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * | @Author :  Sina Darvishi
 * | @Website : R2T.IR
 * |
**/
/* Includes ------------------------------------------------------------------*/

#if defined(STM32F407xx)
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#elif defined(STM32F207xx)
#include "stm32f2xx_hal.h"
#include "stm32f2xx_hal_i2c.h"
#endif

#include <string.h>
#include <stdio.h>

#include "i2c.h"

#include "main.h"		//	hi2c1

//========================================================================
void I2C_BusScan( I2C_HandleTypeDef *phi2c )
//========================================================================
{

	printf( "Scanning I2C bus:\r\n" );
	HAL_StatusTypeDef result;
	uint8_t i;
	for ( i = 1; i < 128; i++ )
	{
		/*
		 * the HAL wants a left aligned i2c address
		 * &hi2c1 is the handle
		 * (uint16_t)(i<<1) is the i2c address left aligned
		 * retries 2
		 * timeout 2
		 */
		result = HAL_I2C_IsDeviceReady( phi2c, (uint16_t)( i << 1 ), 2, 2 );
		if ( result != HAL_OK ) // HAL_ERROR or HAL_BUSY or HAL_TIMEOUT
		{
			printf( "." ); // No ACK received at that address
		}
		if ( result == HAL_OK )
		{
			printf( "0x%X", i ); // Received an ACK at that address
		}
	}
	printf( "\r\n" );
}


//========================================================================
int i2cdetect	( int nBus, uint16_t nDevAddr, uint16_t nDataAddr )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	return 0;
}

//========================================================================
int i2cget		( int nBus, uint16_t nDevAddr, uint16_t nDataAddr )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	return 0;
}

//========================================================================
int i2cset		( int nBus, uint16_t nDevAddr, uint16_t nDataAddr, uint8_t *pData )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	return 0;
}


//int at24_HAL_WriteBytes	(I2C_HandleTypeDef *hi2c,uint16_t DevAddress,uint16_t MemAddress, uint8_t *pData,uint16_t TxBufferSize);
//int at24_HAL_ReadBytes	(I2C_HandleTypeDef *hi2c,uint16_t DevAddress,uint16_t MemAddress, uint8_t *pData,uint16_t RxBufferSize);

//int at24_HAL_SequentialRead(I2C_HandleTypeDef *hi2c ,uint8_t DevAddress,uint16_t MemAddress,uint8_t *pData,uint16_t RxBufferSize);
//int at24_HAL_EraseMemFull(I2C_HandleTypeDef *hi2c);
//int at24_HAL_WriteString(I2C_HandleTypeDef *hi2c,char *pString ,uint16_t MemAddress ,uint8_t length);
//int at24_HAL_ReadString(I2C_HandleTypeDef *hi2c,char *pString,uint16_t MemAddress,uint8_t length);

//========================================================================
void TestEEPROM( I2C_HandleTypeDef *hi2c )
//========================================================================
{
	char d[3]={7,5,10};
	char c[3]={20,20,20};
	
	printf("%s - 0x%02X, 0x%02X, 0x%02X\r\n", __func__, c[0],c[1],c[2]);
	at24_HAL_ReadBytes(hi2c, 0xA0, 0, (uint8_t *)c, 3);
	printf("%s - 0x%02X, 0x%02X, 0x%02X\r\n", __func__, c[0],c[1],c[2]);
	
	at24_HAL_WriteBytes(hi2c, 0xA0, 0, (uint8_t *)d, 3);

	at24_HAL_ReadBytes(hi2c, 0xA0, 0, (uint8_t *)c, 3);
	printf("%s - 0x%02X, 0x%02X, 0x%02X\r\n", __func__, c[0],c[1],c[2]);
}


/**
  * @brief               : This function handles Writing Array of Bytes on the specific Address .
  * 					   This program have this feature that don't force you to use absolute 16 bytes
  * 					   you can use more than 16 bytes buffer.
  * @param  hi2c         : Pointer to a I2C_HandleTypeDef structure that contains
  *                        the configuration information for the specified I2C.
  * @param	DevAddress   : specifies the slave address to be programmed(EEPROM ADDRESS).
  * @param	MemAddress   : Internal memory address (WHERE YOU WANNA WRITE TO)
  * @param	pData	     : Pointer to data buffer
  * @param  TxBufferSize : Amount of data you wanna Write
  * @retval
  */
int at24_HAL_WriteBytes( I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t TxBufferSize )
{
	/*
	 * program just get the DevAddress of the Slave (not master) and for the next step
	 * You know that the most of the EEprom address start with 0xA0
	 * give MemAddress for the location you want to write to
	 * give Data buffer so it can write Data on this location
	 */
	 //Note that this function works properly to 31 bytes
	if ( MemAddress + TxBufferSize > 16 )
	{
		//Write to 16bytes
		while ( HAL_I2C_Mem_Write( hi2c, (uint16_t)DevAddress, (uint16_t)MemAddress, I2C_MEMADD_SIZE_16BIT, pData, (uint16_t)16 - MemAddress, 1000 ) != HAL_OK );
//		while ( HAL_I2C_Mem_Write( hi2c, (uint16_t)DevAddress, (uint16_t)MemAddress, I2C_MEMADD_SIZE_8BIT, pData, (uint16_t)16 - MemAddress, 1000 ) != HAL_OK );
		//write remaining bytes
		*pData = *pData + ( 16 - MemAddress );
		while ( HAL_I2C_Mem_Write( hi2c, (uint16_t)DevAddress, (uint16_t)16, I2C_MEMADD_SIZE_16BIT, pData, (uint16_t)( ( MemAddress + TxBufferSize ) - 16 ), 1000 ) != HAL_OK );
//		while ( HAL_I2C_Mem_Write( hi2c, (uint16_t)DevAddress, (uint16_t)16, I2C_MEMADD_SIZE_8BIT, pData, (uint16_t)( ( MemAddress + TxBufferSize ) - 16 ), 1000 ) != HAL_OK );

	}
	else
	{
		while ( ( TxBufferSize - 16 ) > 0 )
		{
			//if your data is more than 16 bytes,you are here
			while ( HAL_I2C_Mem_Write( hi2c, (uint16_t)DevAddress, (uint16_t)MemAddress, I2C_MEMADD_SIZE_16BIT, pData, (uint16_t)16, 1000 ) != HAL_OK );
//			while ( HAL_I2C_Mem_Write( hi2c, (uint16_t)DevAddress, (uint16_t)16, I2C_MEMADD_SIZE_8BIT, pData, (uint16_t)( ( MemAddress + TxBufferSize ) - 16 ), 1000 ) != HAL_OK );
			TxBufferSize -= 16;
			pData += 16;
			MemAddress += 16;
		}
		//remaining data
		while ( HAL_I2C_Mem_Write( hi2c, (uint16_t)DevAddress, (uint16_t)MemAddress, I2C_MEMADD_SIZE_16BIT, pData, (uint16_t)TxBufferSize, 1000 ) != HAL_OK );
//		while ( HAL_I2C_Mem_Write( hi2c, (uint16_t)DevAddress, (uint16_t)MemAddress, I2C_MEMADD_SIZE_8BIT, pData, (uint16_t)TxBufferSize, 1000 ) != HAL_OK );
	}
	return 1;
}


int at24_HAL_ReadBytes( I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t RxBufferSize )
{
	int TimeOut;
	/*
	 * program just get the DevAddress of the Slave (not master) and for the next step
	 * You know that the most of the EEprom address start with 0xA0
	 * get the MemAddress for the location you want to write data on it
	 * get the Data buffer so it can write Data on this location
	 */
	 //Note that this function works properly to 31bytes

	while ( ( RxBufferSize - 16 ) > 0 )
	{
		//if your data is more than 16 bytes,you are here
		TimeOut = 0;
		while ( HAL_I2C_Mem_Read( hi2c, (uint16_t)DevAddress, (uint16_t)MemAddress, I2C_MEMADD_SIZE_16BIT, pData, (uint16_t)16, 1000 ) != HAL_OK && TimeOut < 10 )
//		while ( HAL_I2C_Mem_Read( hi2c, (uint16_t)DevAddress, (uint16_t)MemAddress, I2C_MEMADD_SIZE_8BIT, pData, (uint16_t)16, 1000 ) != HAL_OK && TimeOut < 10 )
		{
			TimeOut++;
		}

		RxBufferSize -= 16;
		pData += 16;
		MemAddress += 16;
	}
	//			//remaining data
	TimeOut = 0;
	while ( HAL_I2C_Mem_Read( hi2c, (uint16_t)DevAddress, (uint16_t)MemAddress, I2C_MEMADD_SIZE_16BIT, pData, (uint16_t)RxBufferSize, 1000 ) != HAL_OK && TimeOut < 10 )
//	while ( HAL_I2C_Mem_Read( hi2c, (uint16_t)DevAddress, (uint16_t)MemAddress, I2C_MEMADD_SIZE_8BIT, pData, (uint16_t)RxBufferSize, 1000 ) != HAL_OK && TimeOut < 10 )
	{
		TimeOut++;
	}

	return 1;
}

#if 0

 /*
  * @brief               : This function handles Reading Array of Bytes from the specific Address .
  * 					   This program have this feature that don't force you to use absolute 16 bytes
  * 					   you can use more than 16 bytes buffer.
  * @param  hi2c         : Pointer to a I2C_HandleTypeDef structure that contains
  *                        the configuration information for the specified I2C.
  * @param	DevAddress   : specifies the slave address to be programmed(EEPROM ADDRESS).
  * @param	MemAddress   : Internal memory address (WHERE YOU WANNA READ FROM)
  * @param	pData	     : Pointer to data buffer
  * @param  TxBufferSize : Amount of data to be Read
  * @retval
  */
int at24_HAL_SequentialRead(I2C_HandleTypeDef *hi2c ,uint8_t DevAddress,uint16_t MemAddress,uint8_t *pData,uint16_t RxBufferSize)
{
	/*
	 * just like WriteByte but get what it want
	 * but maybe you should know that the Data is location you want to save data
	 * for future use
	 */
		while( (RxBufferSize-16)>0)
		{
			while(HAL_I2C_Mem_Read(hi2c, (uint16_t)DevAddress,(uint16_t)MemAddress,I2C_MEMADD_SIZE_8BIT,pData, (uint16_t)16, (uint32_t)1000)!= HAL_OK);
			RxBufferSize-=16;
			pData+=16;
			MemAddress+=16;
		}
		while(HAL_I2C_Mem_Read(hi2c, (uint16_t)DevAddress,(uint16_t)MemAddress,I2C_MEMADD_SIZE_8BIT,pData, (uint16_t)RxBufferSize, (uint32_t)1000)!= HAL_OK);
        /*
         * if DataRecive  is 0xFF or 255 ,this means that block was empty
         */
	   return 1;
}

 /*
  * @brief               : This function handles Erase Full chip.
  * @param  hi2c         : Pointer to a I2C_HandleTypeDef structure that contains
  *                        the configuration information for the specified I2C.
  * @retval
  */

int at24_HAL_EraseMemFull(I2C_HandleTypeDef *hi2c)
{
	/*
	 * this may take will don't panic
	 */
	uint8_t EraseBuf[16] ={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
	int i;
	for (i =0 ; i<1024 ; i+16)
	{
		/*
		 * if you know,0xFF means that block is empty
		 */
		HAL_I2C_Mem_Write(hi2c,0xA0,(uint16_t )i,EraseBuf,16);
	}
	return 1;
}

/**
  * @brief               : This function handles Writing String on the specific Address .
  * 					   This program have this feature that don't force you to use absolute 16 bytes
  * 					   you can use more than 16 bytes buffer.
  * @param  hi2c         : Pointer to a I2C_HandleTypeDef structure that contains
  *                        the configuration information for the specified I2C.
  * @param	DevAddress   : specifies the slave address to be programmed(EEPROM ADDRESS).
  * @param	MemAddress   : Internal memory address (WHERE YOU WANNA Write)
  * @param	pString	     : Pointer to data buffer(CHAR DATA)
  * @param  length       : Amount of buffer you wanna Write from
  * @retval
  */

int at24_HAL_WriteString(I2C_HandleTypeDef *hi2c,char *pString ,uint16_t MemAddress ,uint8_t length)
{
	uint8_t pData[length];
	int i =0;
	while(*pString)
		(pData[i++])=(uint8_t)(*pString++);
	HAL_I2C_Mem_Write(hi2c,0xA0,MemAddress,pData,length);
	return 1;
}

/**
  * @brief               : This function handles Reading String on the specific Address .
  * 					   This program have this feature that don't force you to use absolute 16 bytes
  * 					   you can use more than 16 bytes buffer.
  * @param  hi2c         : Pointer to a I2C_HandleTypeDef structure that contains
  *                        the configuration information for the specified I2C.
  * @param	MemAddress   : Internal memory address (WHERE YOU WANNA READ)
  * @param	pString	     : Pointer to data buffer(CHAR DATA)
  * @param  length       : Amount of buffer you wanna Read from
  * @retval
  */

int at24_HAL_ReadString(I2C_HandleTypeDef *hi2c,char *pString,uint16_t MemAddress,uint8_t length)
{
	uint8_t pData[length];
	int i=0;
	HAL_I2C_Mem_Read(hi2c, 0xA0,MemAddress,pData,length);
	while(pData[i])
		(*pString++)=(char)pData[i++];
	return 1;
}

#endif

