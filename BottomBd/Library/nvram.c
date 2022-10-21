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
#elif defined(STM32F103xE)	//	STM32F103xC
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#endif

#include <string.h>
#include <stdio.h>

#include "nvram.h"		//	eeprom

#include "main.h"		//	hi2c1


//========================================================================
//	printf disable

//========================================================================

//========================================================================
void I2C_BusScan( I2C_HandleTypeDef *phi2c )
//========================================================================
{
	printf( "Scanning I2C bus:\r\n" );
	HAL_StatusTypeDef result;
	uint8_t i;
	for ( i = 1; i < 126; i++ )
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
//========================================================================
int MB85_HAL_WriteBytes( I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t TxBufferSize )
//========================================================================
{
	int TimeOut;

	TimeOut = 0;
	while ( HAL_I2C_Mem_Write( hi2c, (uint16_t)DevAddress, (uint16_t)MemAddress, I2C_MEMADD_SIZE_8BIT, pData, (uint16_t)TxBufferSize, 10 ) != HAL_OK && TimeOut < 5 ) TimeOut++;

	HAL_Delay(30);

	 if(TimeOut == 5)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}


//========================================================================
int MB85_HAL_ReadBytes( I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t RxBufferSize )
//========================================================================
{
	int TimeOut;

	TimeOut = 0;
	while ( HAL_I2C_Mem_Read( hi2c, (uint16_t)DevAddress, (uint16_t)MemAddress, I2C_MEMADD_SIZE_8BIT, pData, (uint16_t)RxBufferSize, 10 ) != HAL_OK && TimeOut < 5 ) TimeOut++;

    if(TimeOut == 5)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}




//========================================================================
void TestNVRAM( I2C_HandleTypeDef *hi2c )
//========================================================================
{
	char d[100]={7,5,10};
	char c[100]={20,20,20};

	printf("%s - 0x%02X, 0x%02X, 0x%02X\r\n", __func__, c[0],c[1],c[2]);
	MB85_HAL_ReadBytes(hi2c, 0xA0, 0x100, (uint8_t *)c, 3);
	printf("%s - 0x%02X, 0x%02X, 0x%02X\r\n", __func__, c[0],c[1],c[2]);

	MB85_HAL_WriteBytes(hi2c, 0xA0, 0x100, (uint8_t *)d, 3);

	MB85_HAL_ReadBytes(hi2c, 0xA0, 0x100, (uint8_t *)c, 3);
	printf("%s - 0x%02X, 0x%02X, 0x%02X\r\n", __func__, c[0],c[1],c[2]);


}


//========================================================================
int cmd_nvramRead		( int argc, char * argv[] )
//========================================================================
{
    //	eepr [address]
    int 	nAddr = 0;
    uint8_t	buf[10];

    switch ( argc )
    {
    case 2:		sscanf( argv[1], "%d", &nAddr );		//	cmd [address]
        break;
    }

	MB85_HAL_ReadBytes(	&hi2c2,
						MB85RC64_ID,	//	0xA0
						nAddr, (uint8_t *)buf, 1);

	printf("[0x%04X] 0x%02X\n", nAddr, buf[0]);
}

//========================================================================
int cmd_nvramWrite	( int argc, char * argv[] )
//========================================================================
{
    //	eepw [address] [value]
    int 	nAddr = 0;
    int		nVal = 0;
    uint8_t	buf[10];

    switch ( argc )
    {
    case 3:		sscanf( argv[2], "%d", &nVal );			//	cmd [address] [value]
    case 2:		sscanf( argv[1], "%d", &nAddr );		//	cmd [address]
        break;
    }

    buf[0] = nVal;

    printf("[0x%04X] 0x%02X\n", nAddr, buf[0]);

	MB85_HAL_WriteBytes(&hi2c2,
						MB85RC64_ID,	//0xA0,
						nAddr, (uint8_t *)buf, 1);

	printf("[0x%04X] 0x%02X\n", nAddr, buf[0]);
}


//========================================================================
int cmd_nvramDump		( int argc, char * argv[] )
//========================================================================
{
    //	eepr [address]
	int		nAddr = 0;
    int 	nSize = 256;

    int		i;

    uint8_t	buf[256];		//	Page

    switch ( argc )
    {
    case 3:		sscanf( argv[2], "%d", &nSize );		//	cmd [address] [Size]
    case 2:		sscanf( argv[1], "%d", &nAddr );		//	cmd [address]
        break;
    }

	MB85_HAL_ReadBytes(&hi2c2, 0xA0, nAddr, (uint8_t *)buf, nSize);

	printf("[0x%04X] size : %d\n", nAddr, nSize);

	for( i = 0; i < nSize; i++ )
	{
		printf("0x%02X ", buf[i]);
		if ( ((i+1)%16) == 0 ) printf("\n");
	}
}


