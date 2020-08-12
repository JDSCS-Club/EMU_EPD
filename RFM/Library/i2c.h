/*
 * at24_hal_i2c.h
 *
 *  Created on: Sep,11,2015
 *      Author: Sina Darvishi
 */

#ifndef DRIVERS_MYLIB_AT24_HAL_I2C_H_
#define DRIVERS_MYLIB_AT24_HAL_I2C_H_

#if defined(STM32F407xx)
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#elif defined(STM32F207xx)
#include "stm32f2xx_hal.h"
#include "stm32f2xx_hal_i2c.h"
#endif


void I2C_BusScan( I2C_HandleTypeDef *phi2c );


int i2cdetect	( int nBus, uint16_t nDevAddr, uint16_t nDataAddr );

int i2cget		( int nBus, uint16_t nDevAddr, uint16_t nDataAddr );
int i2cset		( int nBus, uint16_t nDevAddr, uint16_t nDataAddr, uint8_t *pData );

int at24_HAL_WriteBytes (I2C_HandleTypeDef *hi2c,uint16_t DevAddress,uint16_t MemAddress, uint8_t *pData,uint16_t TxBufferSize);
int at24_HAL_ReadBytes  (I2C_HandleTypeDef *hi2c,uint16_t DevAddress,uint16_t MemAddress, uint8_t *pData,uint16_t RxBufferSize);

#if 0

int at24_HAL_SequentialRead(I2C_HandleTypeDef *hi2c ,uint8_t DevAddress,uint16_t MemAddress,uint8_t *pData,uint16_t RxBufferSize);
int at24_HAL_EraseMemFull(I2C_HandleTypeDef *hi2c);
int at24_HAL_WriteString(I2C_HandleTypeDef *hi2c,char *pString ,uint16_t MemAddress ,uint8_t length);
int at24_HAL_ReadString(I2C_HandleTypeDef *hi2c,char *pString,uint16_t MemAddress,uint8_t length);

#endif

void TestEEPROM( I2C_HandleTypeDef *hi2c );



#endif /* DRIVERS_MYLIB_AT24_HAL_I2C_H_ */
