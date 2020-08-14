/*------------------------------------------------------------------------------------------
	Project			: LED
	Description		: UART/USART와  관련된 함수들

	Writer			: $Author: zlkit $
	Revision		: $Rev: 1891 $
	Date			: 2019. 01.
	Copyright		: Piranti Corp. ( zlkit@piranti.co.kr )
	 
	Revision History 
	1. 2019. 01.	: Created
-------------------------------------------------------------------------------------------*/

#ifndef	_SERIAL_H_
#define	_SERIAL_H_

//=============================================================================
#if defined(_WIN32)
//=============================================================================

#include "x86_hal_driver.h"

//=============================================================================
#else	//	stm32f207
//=============================================================================

#if defined(STM32F407xx)
#include "stm32f4xx_hal.h"
#elif defined(STM32F207xx)
#include "stm32f2xx_hal.h"
#endif

//=============================================================================
#endif	//	stm32f207
//=============================================================================

#include "QBuf.h"

#define	RXFLAG_EMPTY	0
#define	RXFLAG_XING		1
#define	RXFLAG_VALID	2
#define	IDLE_CHECK_TICK	3	/* 3ms이상 수신이 없으면 frame 종료로 본다 */

#define	RTS_LOW		GPIO_PIN_RESET
#define	RTS_HIGH	GPIO_PIN_SET

int		input_check		( void );

void	SerialInit		( UART_HandleTypeDef *_phuart1, UART_HandleTypeDef *_phuart2 );

extern Queue_t			g_qDebug, g_qRS485;

#endif	/* _SERIAL_H_ */
