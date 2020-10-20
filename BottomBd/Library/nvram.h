/*
 * at24_hal_i2c.h
 *
 *  Created on: Sep,11,2015
 *      Author: Sina Darvishi
 */

#ifndef NVRAM_I2C_H
#define NVRAM_I2C_H

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

/*
 * The MB85RC64T is an FRAM (Ferroelectric Random Access Memory) chip in a configuration of 8,192
 * words × 8 bits, using the ferroelectric process and silicon gate CMOS process technologies for forming the
 * nonvolatile memory cells ( NVRAM )
 */

#define MB85RC64_ID  0xA0

enum eAddrExtMem
{
	AddrExtUpgrMGN1		=	0x20,		//	Upgrade Magic Num ( 0xAA )
	AddrExtUpgrMGN2		=	0x21,		//	Upgrade Magic Num ( 0x55 )

	AddrExtUpgrSize8	=	0x22,		//	Upgrade App Size 0x0000XX
	AddrExtUpgrSize16	=	0x23,		//	Upgrade App Size 0x00XX00
	AddrExtUpgrSize24	=	0x24,		//	Upgrade App Size 0xXX0000
};

void I2C_BusScan( I2C_HandleTypeDef *phi2c );

int MB85_HAL_WriteBytes (I2C_HandleTypeDef *hi2c,uint16_t DevAddress,uint16_t MemAddress, uint8_t *pData,uint16_t TxBufferSize);
int MB85_HAL_ReadBytes  (I2C_HandleTypeDef *hi2c,uint16_t DevAddress,uint16_t MemAddress, uint8_t *pData,uint16_t RxBufferSize);

void TestNVRAM( I2C_HandleTypeDef *hi2c );

int cmd_nvramRead		( int argc, char * argv[] );
int cmd_nvramWrite		( int argc, char * argv[] );
int cmd_nvramDump		( int argc, char * argv[] );

#endif /* NVRAM_I2C_H */
