
#include <string.h>		//	memcpy()

#include "main.h"


SPI_HandleTypeDef *_pSpi;


void radio_spi_init(SPI_HandleTypeDef *hspi)
{
	_pSpi = hspi;
}

void spi_readData(uint8_t size , uint8_t *pData)
{
	HAL_SPI_Receive(_pSpi, pData, size, HAL_MAX_DELAY);
}

void spi_writeData(uint8_t size , uint8_t *pData)
{
	HAL_SPI_Transmit(_pSpi, pData, size, HAL_MAX_DELAY);
}


uint8_t spi_readWrite(uint8_t data)
{
	uint8_t rdata;
	HAL_SPI_TransmitReceive(_pSpi, &data, &rdata, 1 ,HAL_MAX_DELAY);
	return rdata;
}

uint8_t read_cts(uint8_t *pData)
{
	uint8_t cmd[2];
	uint8_t state[2];

	cmd[0] = 0x44;
	cmd[1] = 0x00;

	HAL_GPIO_WritePin( SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(_pSpi, cmd, state, 2,HAL_MAX_DELAY);
	HAL_GPIO_WritePin( SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_SET);

	return state[1];
}

void read_fifo(uint8_t *pData)
{
	uint8_t cmd[65];
//	uint8_t rdata[65];

	HAL_GPIO_WritePin( SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(_pSpi, cmd, pData, sizeof(cmd),HAL_MAX_DELAY);
	HAL_GPIO_WritePin( SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_SET);
}

void write_fifo(uint8_t *pData)
{
	uint8_t cmd[65];
//	uint8_t rdata[65];
	memcpy((cmd+1), pData, 65);

	HAL_GPIO_WritePin( SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(_pSpi, cmd , 65, HAL_MAX_DELAY);
	HAL_GPIO_WritePin( SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_SET);
}


