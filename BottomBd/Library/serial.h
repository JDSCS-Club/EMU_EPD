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
#elif defined(STM32F103xE)	//	STM32F103xC
#include "stm32f1xx_hal.h"
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

extern uint8_t		dataRx1[8];		//	UART1 - Console	- 115200
extern uint8_t		dataRx2[8];		//	UART2 - RFM		- 115200
extern uint8_t		dataRx3[8];		//	UART3 - RS485	- 38400
extern uint8_t		dataRx5[8];		//	UART5 - RS485	- 38400

extern uint32_t		g_nStampRx1;		//	Last Data Recv TimeStamp
extern uint32_t		g_nStampRx2;
extern uint32_t		g_nStampRx3;
extern uint32_t		g_nStampRx5;


void	rts_high		( UART_HandleTypeDef *_phuart );
void	rts_low			( UART_HandleTypeDef *_phuart );

void	resetSerial		( UART_HandleTypeDef* uart );
void	checkSerial		( UART_HandleTypeDef* uart );

int		input_check		( void );

void	SerialInit		( UART_HandleTypeDef *_phuart1, UART_HandleTypeDef *_phuart2, UART_HandleTypeDef *_phuart3, UART_HandleTypeDef *_phuart5  );

extern void checkSerial(UART_HandleTypeDef* uart);

extern	Queue_t		g_qUart1;
extern	Queue_t		g_qUart2;
extern	Queue_t		g_qUart3;
extern	Queue_t		g_qUart5;



#endif	/* _SERIAL_H_ */
