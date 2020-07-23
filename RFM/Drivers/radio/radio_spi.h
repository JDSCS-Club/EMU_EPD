
#ifndef RADIO_SPI_H
#define	RADIO_SPI_H

#include <stdint.h>		//	uint8_t

//#include <stm32f4xx_hal_spi.h>		//	SPI_HandleTypeDef
#include "main.h"		//	SPI_HandleTypeDef

void radio_spi_init(SPI_HandleTypeDef *hspi);

void spi_readData(uint8_t size, uint8_t *pData);

void spi_writeData(uint8_t size, uint8_t *pData);

uint8_t spi_readWrite(uint8_t data);

uint8_t read_cts(uint8_t *pData);

void read_fifo(uint8_t *pData);

void write_fifo(uint8_t *pData);


#endif	//	RADIO_SPI_H
