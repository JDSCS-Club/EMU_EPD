/*
 * at24_hal_i2c.h
 *
 *  Created on: Sep,11,2015
 *      Author: Sina Darvishi
 */

#ifndef EEPROM_I2C_H
#define EEPROM_I2C_H

#if defined(STM32F407xx)
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#elif defined(STM32F207xx)
#include "stm32f2xx_hal.h"
#include "stm32f2xx_hal_i2c.h"
#endif


void I2C_BusScan( I2C_HandleTypeDef *phi2c );


int M24_HAL_WriteBytes (I2C_HandleTypeDef *hi2c,uint16_t DevAddress,uint16_t MemAddress, uint8_t *pData,uint16_t TxBufferSize);
int M24_HAL_ReadBytes  (I2C_HandleTypeDef *hi2c,uint16_t DevAddress,uint16_t MemAddress, uint8_t *pData,uint16_t RxBufferSize);


void TestEEPROM( I2C_HandleTypeDef *hi2c );


int cmd_eepromRead		( int argc, char * argv[] );
int cmd_eepromWrite		( int argc, char * argv[] );
int cmd_eepromDump		( int argc, char * argv[] );

#endif /* EEPROM_I2C_H */
